/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QtCore/QThread>
#include <QtCore/QAtomicInt>
#include <QtCore/QMutexLocker>
#include <QtCore/QMutex>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>
#include <QtCore/QTimer>

#include <Qt3DCore/private/qaspectjobmanager_p.h>
#include <Qt3DCore/private/qabstractaspectjobmanager_p.h>
#include <Qt3DCore/qaspectjob.h>
#include <Qt3DCore/qt3dcore_global.h>

// Add DEFINES += QT_BUILD_INTERNAL at least to Qt3d's core.pro
// when running these tests. It makes QAspectJobManager available.

class tst_ThreadPooler : public QObject
{
    Q_OBJECT

public:
    tst_ThreadPooler() {}
    ~tst_ThreadPooler() {}

private:
    Qt3D::QAspectJobManager *m_jobManager;

private slots:
    void init();
    void cleanup();

    void defaultPerThread();
    void defaultAspectQueue();
    void doubleAspectQueue();
    void dependencyAspectQueue();
    void massTest();
};

typedef Qt3D::QAspectJobManager JobManager;
typedef void (*TestFunction)(QAtomicInt *, int *);
typedef void (*MassFunction)(QVector3D *data);

void perThreadFunction(void *arg)
{
    ((QAtomicInt *)arg)->ref();
}


// General test AspectJob

class TestAspectJob : public Qt3D::QAspectJob
{
public:
    TestAspectJob(TestFunction func, QAtomicInt *counter, int *value);

    void setMutex(QMutex *mutex);

    void run() Q_DECL_OVERRIDE;

private:
    TestFunction m_func;
    QAtomicInt *m_counter;
    int *m_value;
    QMutex *m_mutex;
};

TestAspectJob::TestAspectJob(TestFunction func, QAtomicInt *counter, int *value)
    : m_func(func),
      m_counter(counter),
      m_value(value)
{
}

void TestAspectJob::setMutex(QMutex *mutex)
{
    m_mutex = mutex;
}

void TestAspectJob::run()
{
    m_func(m_counter, m_value);
}

// Mass test AspectJob

class MassAspectJob : public Qt3D::QAspectJob
{
public:
    MassAspectJob(MassFunction func, QVector3D *data);

    void run() Q_DECL_OVERRIDE;

private:
    MassFunction m_func;
    QVector3D *m_data;
};

MassAspectJob::MassAspectJob(MassFunction func, QVector3D *data)
    : m_func(func),
      m_data(data)
{
}

void MassAspectJob::run()
{
    m_func(m_data);
}

void incrementFunctionCallCounter(QAtomicInt *counter, int *value)
{
    Q_UNUSED(value);

    counter->ref();
}

void add2(QAtomicInt *counter, int *value)
{
    Q_UNUSED(counter);

    // Sleep for a while so that we see that multiply task really
    // wait for us
    QThread::currentThread()->msleep(400);
    *value = *value + 2;
}

void multiplyBy2(QAtomicInt *counter, int *value)
{
    Q_UNUSED(counter);

    *value = *value * 2;
}

void massTestFunction(QVector3D *data)
{
    QVector3D point(4.5f, 4.5f, 4.5f);

    QMatrix4x4 matrix;
    matrix.lookAt(QVector3D(10.0f, 1.5f, 2.0f), QVector3D(1.0f, -1.0f, 1.0f),
                  QVector3D(0.0f, 0.0f, 1.0f));
    QVector3D result = matrix.map(point);
    data->setX(result.x());
    data->setY(result.y());
    data->setZ(result.z());
}

void tst_ThreadPooler::init()
{
    m_jobManager = new JobManager(this);
}

void tst_ThreadPooler::cleanup()
{
    delete m_jobManager;
}

void tst_ThreadPooler::defaultPerThread()
{
    // GIVEN
    QAtomicInt callCounter;
    int maxThreadCount = QThread::idealThreadCount();
    callCounter.store(0);

    // WHEN
    m_jobManager->waitForPerThreadFunction(perThreadFunction, &callCounter);

    // THEN
    QVERIFY(maxThreadCount == callCounter.load());
}

void tst_ThreadPooler::defaultAspectQueue()
{
    // GIVEN
    QAtomicInt callCounter;
    int value = 0; // Not used in this test
    QVector<QSharedPointer<Qt3D::QAspectJob> > jobList;
    callCounter.store(0);
    const int jobCount = 5;

    // WHEN
    for (int i = 0; i < jobCount; i++) {
        QSharedPointer<TestAspectJob> job(new TestAspectJob(incrementFunctionCallCounter,
                                                            &callCounter, &value));
        jobList.append(job);
    }
    m_jobManager->enqueueJobs(jobList);
    m_jobManager->waitForAllJobs();

    // THEN
    QVERIFY(jobCount == callCounter.load());
}

/*
 * Feeds two list of jobs to queue. The pooler should be able to add jobs on
 * the second list to execution. Single call to wait finish.
 */
void tst_ThreadPooler::doubleAspectQueue()
{
    // GIVEN
    QAtomicInt callCounter;
    int value = 0; // Not used in this test
    QVector<QSharedPointer<Qt3D::QAspectJob> > jobList;
    callCounter.store(0);
    const int jobCount = 3;

    // WHEN
    for (int i = 0; i < jobCount; i++) {
        QSharedPointer<TestAspectJob> job(new TestAspectJob(incrementFunctionCallCounter,
                                                            &callCounter, &value));
        jobList.append(job);
    }
    m_jobManager->enqueueJobs(jobList);

    QVector<QSharedPointer<Qt3D::QAspectJob> > jobList2;
    for (int i = 0; i < jobCount; i++) {
        QSharedPointer<TestAspectJob> job(new TestAspectJob(incrementFunctionCallCounter,
                                                            &callCounter, &value));
        jobList2.append(job);
    }
    m_jobManager->enqueueJobs(jobList2);

    m_jobManager->waitForAllJobs();

    // THEN
    QVERIFY(jobCount * 2 == callCounter.load());
}

/*
 * Default test for jobs that have dependencies.
 */
void tst_ThreadPooler::dependencyAspectQueue()
{
    // GIVEN
    QAtomicInt callCounter; // Not used in this test
    int value = 2;
    QVector<QSharedPointer<Qt3D::QAspectJob> > jobList;

    // WHEN
    QSharedPointer<TestAspectJob> job1(new TestAspectJob(add2, &callCounter, &value));
    jobList.append(job1);
    QSharedPointer<TestAspectJob> job2(new TestAspectJob(multiplyBy2, &callCounter, &value));
    job2->addDependency(job1);
    jobList.append(job2);
    m_jobManager->enqueueJobs(jobList);
    m_jobManager->waitForAllJobs();

    // THEN
    // value should be (2+2)*2 = 8
    QVERIFY(value == 8);
}

void tst_ThreadPooler::massTest()
{
    // GIVEN
    const int mass = 600; // 600
    QVector<QSharedPointer<Qt3D::QAspectJob> > jobList;
    QVector3D data[3 * mass];

    // WHEN
    QTime time;
    time.start();

    for (int i = 0; i < mass; i++) {
        QSharedPointer<MassAspectJob> job1(new MassAspectJob(massTestFunction, &(data[i * 3 + 0])));
        jobList.append(job1);
        QSharedPointer<MassAspectJob> job2(new MassAspectJob(massTestFunction, &(data[i * 3 + 1])));
        job2->addDependency(job1);
        jobList.append(job2);
        QSharedPointer<MassAspectJob> job3(new MassAspectJob(massTestFunction, &(data[i * 3 + 2])));
        job3->addDependency(job2);
        jobList.append(job3);
    }

    m_jobManager->enqueueJobs(jobList);
    m_jobManager->waitForAllJobs();

    // THEN
    qDebug() << "time.elapsed() = " << time.elapsed() << " ms";
}

QTEST_APPLESS_MAIN(tst_ThreadPooler)

#include "tst_threadpooler.moc"
