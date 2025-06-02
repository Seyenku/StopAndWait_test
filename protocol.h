#pragma once
#include "common.h"

// Структуры данных протокола
struct Packet {
    int sequence_number;
    char data[256];

    Packet();
    Packet(int seq, const string& message);
};

struct Acknowledgement {
    int ack_number;

    Acknowledgement();
    explicit Acknowledgement(int ack);
};

// Канал связи
class Channel {
private:
    queue<Packet> packet_queue;
    queue<Acknowledgement> ack_queue;
    mutex mtx;
    condition_variable cv_packet;
    condition_variable cv_ack;

    mt19937 rng;
    uniform_real_distribution<float> dist;
    float loss_probability;

public:
    Channel(float loss_prob = 0.05f);

    bool isPacketLost();
    void sendPacket(const Packet& packet);
    Packet receivePacket();
    bool hasPacket();
    void sendAck(const Acknowledgement& ack);
    Acknowledgement receiveAck();
    bool hasAck();
    void setLossProbability(float prob);
    float getLossProbability() const;
};

// Отправитель
class Sender {
private:
    SenderState state;
    int current_seq_number;
    Packet current_packet;
    Channel& channel;
    int timeout_ms;
    bool running;
    thread sender_thread;
    chrono::steady_clock::time_point last_send_time;

public:
    Sender(Channel& ch, int timeout = 1000);
    ~Sender();

    void sendData(const string& data);
    void receiveAck();
    void handleTimeout();
    void start();
    void stop();
    bool isReady() const;
    SenderState getState() const;
    int getCurrentSeqNumber() const;
    void setState(SenderState newState);
    void setSeqNumber(int seq);
};

// Получатель
class Receiver {
private:
    ReceiverState state;
    int expected_seq_number;
    Channel& channel;
    bool running;
    thread receiver_thread;

public:
    Receiver(Channel& ch);
    ~Receiver();

    void receivePacket();
    void start();
    void stop();
    ReceiverState getState() const;
    int getExpectedSeqNumber() const;
    void setState(ReceiverState newState);
    void setExpectedSeqNumber(int seq);
};