#include "WorkerThread.h"
#include "TrtModelWorker.h"
#include <string>
#include "cpp-httplib/httplib.h"
#include <memory>

class MessageWorker: public WorkerThread
{
public:
    MessageWorker(const std::string& threadName, const std::string& ip, const int& port, const int& threadNum, 
                const std::vector<std::shared_ptr<TrtModelWorker>>& trtModelWorkers);
    ~MessageWorker();
private:
    void run() override;
private:
    std::string mIp;
    int mPort;
    int mThreadNum;
    std::vector<std::shared_ptr<TrtModelWorker>> mTrtModelWorkers;
    // httplib::Client* mClient;
};
