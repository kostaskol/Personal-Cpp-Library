#ifndef TASK_H
#define TASK_H

class task {
public:
    task() = default;
    virtual ~task() = default;

    virtual void run() = 0;
};

#endif // TASK_H
