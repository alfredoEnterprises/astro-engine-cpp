#ifndef AE_CHART_HANDLER_H
#define AE_CHART_HANDLER_H

#include <aws/lambda-runtime/runtime.h>

aws::lambda_runtime::invocation_response
chart_handler(aws::lambda_runtime::invocation_request const& req);

#endif
