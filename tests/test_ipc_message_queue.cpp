#include <gtest/gtest.h>
#include "ipc_message_queue.h"


TEST(MessageQueueTest, CreateQueueSuccess) {
    ipc::MessageQueue mq;
    int result = mq.Create("/test_queue", 128, 10);
    ASSERT_EQ(result, true);  // Expect file descriptor to be greater than 0 on success
}
