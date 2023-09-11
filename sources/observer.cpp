// Copyright 2023 AndreevSemen semen.andreev00@mail.ru

#include <config.hpp>
#include <behavior.hpp>
#include <matrix.hpp>


int main(int argc, char** argv) {
    Config config{argv[1], argv[2], argv[3]};

    boost::asio::io_service ioService;
    port_t port = (config.Containerization()) ? config.DockerPort() : config.Port();
    Socket socket{ioService, port, config.BufferSize()};

    ThreadSaveQueue<PullGossip> pullQ;
    socket.RunObserving(pullQ);

    MemberMatrix matrix;
    while (true) {
        auto pulls = pullQ.Pop(config.ObserveNum());

        for (auto& pull : pulls) {
            matrix.Push(std::move(pull));
        }

        static TimeStamp logRepetition = TimeStamp::Now();
        if (logRepetition.TimeDistance(TimeStamp::Now()) > config.LogRepetition()) {
            matrix.Log(std::cout);

            logRepetition = TimeStamp::Now();
        }

        matrix.DetectFailure(config.ObserverFDTimout());


    }
}
