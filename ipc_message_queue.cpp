#include "ipc_message_queue.h"
#include <vector>
#include <cstring> // For std::strncpy
#include <stdexcept>
#include <optional>

namespace ipc {

MessageQueue::MessageQueue() noexcept
{
}

MessageQueue::~MessageQueue() noexcept
{
    if (!queue_name.empty()) {
        mq_unlink(queue_name.c_str());
    }
    if (state == State::Created) {
        mq_close(mq.value());
    }
}

bool MessageQueue::Create(const std::string& queue_name, int max_message_size, int max_message_count)
{
    struct mq_attr attr {};
    attr.mq_flags = 0;
    attr.mq_maxmsg = max_message_count;
    attr.mq_msgsize = max_message_size;
    attr.mq_curmsgs = 0;

    this->max_message_size = max_message_size;
    mq_unlink(queue_name.c_str());

    mqd_t mq_descriptor = mq_open(queue_name.c_str(), O_CREAT | O_RDWR | O_NONBLOCK, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, &attr);
    if (mq_descriptor >= 0) {
        mq = mq_descriptor;
        struct mq_attr current_attr;
        if (mq_getattr(mq.value(), &current_attr) < 0) {
            return false;
        }
        message_count = current_attr.mq_curmsgs;
        this->queue_name = queue_name;
        state = State::Created;
        return true;
    }
    return false;
}

bool MessageQueue::Open(const std::string& queue_name)
{
    mqd_t mq_descriptor = mq_open(queue_name.c_str(), O_RDWR | O_NONBLOCK);
    if (mq_descriptor >= 0) {
        mq = mq_descriptor;
        struct mq_attr current_attr;
        if (mq_getattr(mq.value(), &current_attr) < 0) {
            return false;
        }
        message_count = current_attr.mq_curmsgs;
        max_message_size = current_attr.mq_msgsize;
        max_message_count = current_attr.mq_maxmsg;
        this->queue_name = queue_name;
        state = State::Opened;
        return true;
    }
    return false;
}

bool MessageQueue::Flush()
{
    std::vector<char> buffer(max_message_size);
    while (message_count > 0) {
        mq_receive(mq.value(), buffer.data(), max_message_size, nullptr);
        struct mq_attr attr;
        if (mq_getattr(mq.value(), &attr) < 0) {
            return false;
        }
        message_count = attr.mq_curmsgs;
    }
    return true;
}

bool MessageQueue::Receive(std::span<char> buffer)
{
    int ret = mq_receive(mq.value(), buffer.data(), max_message_size, nullptr);
    if (ret >= 0) {
        struct mq_attr attr;
        if (mq_getattr(mq.value(), &attr) < 0) {
            return false;
        }
        message_count = attr.mq_curmsgs;
        return true;
    }
    return false;
}

bool MessageQueue::Send(std::span<const char> buffer)
{
    if (buffer.size() <= static_cast<size_t>(max_message_size)) {
        int ret = mq_send(mq.value(), buffer.data(), buffer.size(), 0);
        if (ret == 0) {
            struct mq_attr attr;
            if (mq_getattr(mq.value(), &attr) < 0) {
                return false;
            }
            message_count = attr.mq_curmsgs;
            return true;
        }
    }
    return false;
}

} // namespace ipc
