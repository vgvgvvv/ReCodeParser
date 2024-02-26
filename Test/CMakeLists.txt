
set(TargetName ReCodeParser_Test)
ReMake_AddTarget(
    TARGET_NAME ${TargetName}
    MODE EXE
    INC "Public"
    LIB "ReCodeParser"
        "ReCppCommon"
)
