#include <string>

class Application {

public:
    Application() {}

    virtual ~Application() {}

    virtual void Initialize() = 0;

    virtual void LoadMesh(const std::string& filename) = 0;

    virtual void SaveMesh(const std::string& filename) = 0;
};
