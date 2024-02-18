
set(TargetName ReCodeParser)
ReMake_AddTarget(
    TARGET_NAME ${TargetName}
    MODE SHARED
    INC "Public"
    LIB "ReCppCommon"
        "ReClass"
)
