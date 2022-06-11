from datetime import datetime
from os.path import dirname, join
cwd = dirname(__file__)
now = datetime.now()
dt_string = now.strftime("%d/%m/%Y %H:%M")
print("date and time =", dt_string)
with open(join(cwd, './src/version.c'), 'w') as f:
    chars = ["'{0}'".format(char) for char in dt_string]
    f.write("""
    #ifndef ZCL_APP_VERSION_H
    #define ZCL_APP_VERSION_H

    #ifdef __cplusplus
    extern "C" {
    #endif

    """
    )
    f.write('#include "version.h"\n')
    code = """const uint8 zclApp_DateCode[] = {{ {0}, {1} }};\n""".format(len(chars), ', '.join(chars))
    f.write(code)
    code = """const char zclApp_DateCodeNT[] = \"{0}\";\n""".format(dt_string)
    f.write(code)
    f.write("""
    #ifdef __cplusplus
    }
    #endif

    #endif /* ZCL_APP_VERSION_H */
    """)