#include <stdio.h>
#include "option.h"
#include "test_plan.h"

int main(int argc, const char *argv[])
{
    int result;
    int status;

    status = option_init(argc, argv);
    if (status)
    {
        return status;
    }

    printf("GDFlashSim Start!\n");
    result = TestPlan(test_config_file, test_param_file);
    getchar();

    return result;
}




