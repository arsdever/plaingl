#pragma once

void gl_error_handler(unsigned source,
                      unsigned type,
                      unsigned id,
                      unsigned severity,
                      int length,
                      const char* message,
                      const void* user_parameter);
