#include "protocol.h"

// Реализация Packet
Packet::Packet() : sequence_number(0) {
    memset(data, 0, sizeof(data));
}

Packet::Packet(int seq, const string& message) : sequence_number(seq) {
    strncpy_s(data, sizeof(data), message.c_str(), sizeof(data) - 1);
}

// Реализация Acknowledgement
Acknowledgement::Acknowledgement() : ack_number(0) {}

Acknowledgement::Acknowledgement(int ack) : ack_number(ack) {}

// Реализация Channel
Channel::Channel(float loss_prob) : loss_probability(loss_prob) {
    random_device rd;
    rng = mt19937(rd());
    dist = uniform_real_distribution<float>(0.0f, 1.0f);
}

bool Channel::isPacketLost() {
    if (loss_probability < 0.0f || loss_probability > 1.0f) {
        throw invalid_argument("Loss probability must be between 0.0 and 1.0");
    }
    return dist(rng) < loss_probability;
}

void Channel::sendPacket(const Packet& packet) {
    lock_guard<mutex> lock(mtx);
    if (!isPacketLost()) {
        packet_queue.push(packet);
        cv_packet.notify_one();
    }
}

Packet Channel::receivePacket() {
    unique_lock<mutex> lock(mtx);
    cv_packet.wait(lock, [this]() { return !packet_queue.empty(); });

    Packet packet = packet_queue.front();
    packet_queue.pop();
    return packet;
}

bool Channel::hasPacket() {
    lock_guard<mutex> lock(mtx);
    return !packet_queue.empty();
}

void Channel::sendAck(const Acknowledgement& ack) {
    lock_guard<mutex> lock(mtx);
    if (!isPacketLost()) {
        ack_queue.push(ack);
        cv_ack.notify_one();
    }
}

Acknowledgement Channel::receiveAck() {
    unique_lock<mutex> lock(mtx);
    cv_ack.wait(lock, [this]() { return !ack_queue.empty(); });

    Acknowledgement ack = ack_queue.front();
    ack_queue.pop();
    return ack;
}

bool Channel::hasAck() {
    lock_guard<mutex> lock(mtx);
    return !ack_queue.empty();
}

void Channel::setLossProbability(float prob) {
    loss_probability = prob;
}

float Channel::getLossProbability() const {
    return loss_probability;
}

// Реализация Sender
Sender::Sender(Channel& ch, int timeout)
    : state(SenderState::READY),
    current_seq_number(0),
    channel(ch),
    timeout_ms(timeout),
    running(true) {
}

Sender::~Sender() {
    stop();
    if (sender_thread.joinable()) {
        sender_thread.join();
    }
}

void Sender::sendData(const string& data) {
    if (state == SenderState::READY) {
        current_packet = Packet(current_seq_number, data);
        channel.sendPacket(current_packet);
        state = SenderState::WAITING;
        last_send_time = chrono::steady_clock::now();
    }
}

void Sender::receiveAck() {
    if (state == SenderState::WAITING && channel.hasAck()) {
        Acknowledgement ack = channel.receiveAck();
        if (ack.ack_number == current_seq_number) {
            current_seq_number = 1 - current_seq_number;
            state = SenderState::READY;
        }
    }
}

void Sender::handleTimeout() {
    state = SenderState::TIMEOUT;
    channel.sendPacket(current_packet);
    state = SenderState::WAITING;
    last_send_time = chrono::steady_clock::now();
}

void Sender::start() {
    sender_thread = thread([this]() {
        while (running) {
            if (state == SenderState::WAITING) {
                receiveAck();

                if (state == SenderState::WAITING) {
                    auto current_time = chrono::steady_clock::now();
                    auto elapsed = chrono::duration_cast<chrono::milliseconds>(
                        current_time - last_send_time).count();

                    if (elapsed > timeout_ms) {
                        handleTimeout();
                    }
                }
            }
            this_thread::sleep_for(chrono::milliseconds(100));
        }
        });
}

void Sender::stop() {
    running = false;
}

bool Sender::isReady() const {
    return state == SenderState::READY;
}

SenderState Sender::getState() const {
    return state;
}

int Sender::getCurrentSeqNumber() const {
    return current_seq_number;
}

void Sender::setState(SenderState newState) {
    state = newState;
}

void Sender::setSeqNumber(int seq) {
    current_seq_number = seq;
}

// Реализация Receiver
Receiver::Receiver(Channel& ch)
    : state(ReceiverState::WAITING),
    expected_seq_number(0),
    channel(ch),
    running(true) {
}

Receiver::~Receiver() {
    stop();
    if (receiver_thread.joinable()) {
        receiver_thread.join();
    }
}

void Receiver::receivePacket() {
    if (channel.hasPacket()) {
        Packet packet = channel.receivePacket();

        if (packet.sequence_number == expected_seq_number) {
            // Корректный пакет - отправляем ACK и обновляем ожидаемый номер
            Acknowledgement ack(packet.sequence_number);
            channel.sendAck(ack);
            expected_seq_number = 1 - expected_seq_number;
            state = ReceiverState::ACK_SENT;
        }
        else {
            // Дубликат или неправильный номер - отправляем ACK, но не обновляем ожидаемый номер
            Acknowledgement ack(packet.sequence_number);
            channel.sendAck(ack);
            state = ReceiverState::ACK_SENT;
        }
    }
}

void Receiver::start() {
    receiver_thread = thread([this]() {
        while (running) {
            if (state == ReceiverState::WAITING || state == ReceiverState::RECEIVED || state == ReceiverState::ACK_SENT) {
                receivePacket();
                // После обработки пакета возвращаемся в состояние ожидания
                if (state == ReceiverState::ACK_SENT) {
                    state = ReceiverState::WAITING;
                }
            }
            this_thread::sleep_for(chrono::milliseconds(100));
        }
        });
}

void Receiver::stop() {
    running = false;
}

ReceiverState Receiver::getState() const {
    return state;
}

int Receiver::getExpectedSeqNumber() const {
    return expected_seq_number;
}

void Receiver::setState(ReceiverState newState) {
    state = newState;
}

void Receiver::setExpectedSeqNumber(int seq) {
    expected_seq_number = seq;
}