#include <jlm_result.h>

void ResultMiddleware::AllRunBefore() {
    /** 为每组数据创建 result */
    jl_cycle->results = new JudgeLightResult[jl_cycle->run_count + 1];
}

void ResultMiddleware::RunResult(int cnt, struct rusage &ru, int &status) {
    JudgeLightResult rst;

    /** 计算时间与内存占用 */
    rst.time_used = ru.ru_utime.tv_sec * 1000 + ru.ru_utime.tv_usec / 1000 +
                    ru.ru_stime.tv_sec * 1000 + ru.ru_stime.tv_usec / 1000;
    rst.memory_used = ru.ru_maxrss;

    if (WIFSIGNALED(status)) {  // 出现异常
        /* 获得退出原因 */
        switch (WTERMSIG(status)) {
            case SIGSEGV:
                if (rst.memory_used > jl_cycle->memory_limit) {
                    rst.result = MemoryLimitExceeded;
                } else {
                    rst.result = RuntimeError;
                }
                break;
            case SIGALRM:
            case SIGXCPU:
                rst.result = TimeLimitExceeded;
                break;
            default:
                rst.result = RuntimeError;
                break;
        }
        rst.signum = WTERMSIG(status);
        jl_cycle->run_error = true;
    } else {  // 未出现异常
        if (rst.time_used > jl_cycle->time_limit) {
            rst.result = TimeLimitExceeded;
        } else if (rst.memory_used > jl_cycle->memory_limit) {
            rst.result = MemoryLimitExceeded;
        } else {
            rst.result = DiffData(cnt);
        }
    }
    jl_cycle->results[cnt] = rst;
}

int DiffData(int) {
    // TODO
    // 对比输出文件和答案文件
    // 完全相同：AC
    // 除了末尾的换行符外完全相同：AC
    // 除了空格、格式制表符、换行符等空白符外完全相同：PE
    // 其他情况：WA
    return Accepted;
}
