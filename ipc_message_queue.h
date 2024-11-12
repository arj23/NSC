#pragma once

#include <mqueue.h>
#include <memory>
#include <string>
#include <span>

namespace ipc {

class MessageQueue final
{
public:
    explicit MessageQueue();
    ~MessageQueue();
    
    bool Create(const std::string& queue_name, int max_message_size, int max_message_count);
    bool Open(const std::string& queue_name);
    bool Receive(std::span<char> buffer);
    bool Send(std::span<const char> buffer);
    bool Flush();
    
    inline int get_message_count() const { return message_count; }

private:
    std::string queue_name;
    int max_message_size;
    int max_message_count;
    int message_count = 1;
    std::unique_ptr<mqd_t, decltype(&mq_close)> mq {nullptr, mq_close};
    
    enum class State {
        NotInitialized,
        Created,
        Opened
    };
    State state = State::NotInitialized;
};

} // namespace ipc

#endif // IPC_MESSAGE_QUEUE_H
