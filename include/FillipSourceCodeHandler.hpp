#ifndef FILLIP_SOURCE_CODE_HANDLER
#define FILLIP_SOURCE_CODE_HANDLER

class Script {
public:
    Script() {}
    virtual void OnRun() {}
    virtual void OnUpdate(float dt) {}
};

#endif // FILLIP_SOURCE_CODE_HANDLER