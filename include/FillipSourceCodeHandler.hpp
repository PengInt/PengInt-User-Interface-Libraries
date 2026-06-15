#ifndef FILLIP_SOURCE_CODE_HANDLER
#define FILLIP_SOURCE_CODE_HANDLER

class Script {
public:
    Script() {}
    virtual void OnRun() {}
    virtual void OnUpdate(float dt) {}
};

extern Script* UserGameInstance;
#define REGISTER_GAME_CLASS(ClassName) \
    Script* UserGameInstance = new ClassName();

#endif // FILLIP_SOURCE_CODE_HANDLER