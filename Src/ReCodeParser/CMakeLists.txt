
set(TargetName ReCodeParser)
ReMake_AddTarget(
    TARGET_NAME ${TargetName}
    MODE SHARED
    INC "Public"
    INC_PRIVATE "Private"
    LIB "ReCppCommon"
        "ReClass"
)
