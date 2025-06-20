#include <iostream>
#include <memory>

/*
 * 适配器模式：让不兼容的接口可以在一起工作
 * 适配器模式（Adapter Pattern）是一种设计模式，它允许将一个类的接口转换为另一个接口，
 * 从而使原本不兼容的类可以一起工作。简单来说，适配器模式就像是一个“转换器”，它把一个类的接口转换成另一个类期望的接口。
 *
 * 在软件开发中，经常会遇到这样的情况：有一个现成的类（比如第三方库中的类），它的功能很好用，但它的接口和我们系统的接口不兼容。
 * 适配器模式可以帮助我们解决这个问题，而不需要修改原有类的代码。
 *
 * 适配器模式主要有两种实现方式：对象适配器和类适配器。
 *      对象适配器：通过在适配器类中包含一个原始类的实例来实现适配。这种方式使用组合关系，适配器类与原始类是松耦合的。
 *      类适配器：通过让适配器类继承自原始类来实现适配。这种方式使用继承关系，适配器类与原始类是紧耦合的。
 *
 */

// 简单的适配器模式例子
// 新的支付接口（Target）
class PaymentInterface {
public:
    virtual ~PaymentInterface() = default;
    virtual void pay(const std::string& amount) = 0;
};

// 旧的支付系统
class LegacyPaymentSystem {
public:
    void process_payment(const std::string& amount) {
        std::cout << "LegacyPaymentSystem: Processing payment of " << amount << std::endl;
    }
};

// 适配器类（Adapter）- 对象适配器方式实现
class PaymentAdapter : public PaymentInterface {
public:
    void pay(const std::string& amount) override {
        // 适配逻辑：将新的支付接口调用转换为旧的支付系统调用
        legacy_payment_system_.process_payment(amount);
    }
private:
    LegacyPaymentSystem legacy_payment_system_;  // 组合旧的支付系统
};

int main() {
    std::unique_ptr<PaymentInterface> payment(new PaymentAdapter);
    payment->pay("1024.10");
    return 0;
}

#if 0
/**************************** 类适配器的实现 ********************************************/
// 多媒体播放器接口
class MediaPlayer {
public:
    virtual void play(const std::string &audio_type, const std::string &file_name) = 0;
    virtual ~MediaPlayer() = default;
};

// 旧适配的接口
class AdvancedMediaPlayer {
public:
    virtual void play_vlc(const std::string &file_name) = 0;
    virtual void play_mp4(const std::string &file_name) = 0;
    virtual ~AdvancedMediaPlayer() = default;
};

// 具体的适配器实现
class VlcPlayer : public AdvancedMediaPlayer {
public:
    void play_vlc(const std::string &file_name) override {
        std::cout << "Playing vlc file. Name: " << file_name << std::endl;
    }
    void play_mp4(const std::string &file_name) override {
        std::cout << "nothing... " << std::endl;
    }
};

// 具体的适配器实现
class Mp4Player : public AdvancedMediaPlayer {
public:
    void play_vlc(const std::string &file_name) override {
        std::cout << "nothing... " << std::endl;
    }
    void play_mp4(const std::string &file_name) override {
        std::cout << "Playing mp4 file. Name: " << file_name << std::endl;
    }
};

#if 0
// 新的适配器，使用统一的接口，使用多继承的方式，在新接口调用旧接口
class MediaAdapter : public MediaPlayer, public AdvancedMediaPlayer {
public:
    MediaAdapter(const std::string &audio_type) {
        if (audio_type == "vlc") {
            advanced_media_player_ = new VlcPlayer();
        } else if (audio_type == "mp4") {
            advanced_media_player_ = new Mp4Player();
        }
    }
    ~MediaAdapter() {
        if (advanced_media_player_) {
            delete advanced_media_player_;
            advanced_media_player_ = nullptr;
        }
    }
    void play(const std::string &audio_type, const std::string &file_name) override {
        if (audio_type == "vlc") {
            advanced_media_player_->play_vlc(file_name);
        } else if (audio_type == "mp4") {
            advanced_media_player_->play_mp4(file_name);
        }
    }
    void play_mp4(const std::string &file_name) {
        advanced_media_player_->play_mp4(file_name);
    }
    void play_vlc(const std::string &file_name) {
        advanced_media_player_->play_vlc(file_name);
    }

private:
    AdvancedMediaPlayer *advanced_media_player_;
};
#else


/**************************** 对象适配器的实现 ********************************************/

// 新的适配器，使用统一的接口，使用组合的方式，在新接口调用旧接口
class MediaAdapter : public MediaPlayer {
public:
    MediaAdapter(const std::string &audio_type) {
        if (audio_type == "vlc") {
            advanced_media_player_ = new VlcPlayer();
        } else if (audio_type == "mp4") {
            advanced_media_player_ = new Mp4Player();
        }
    }
    ~MediaAdapter() {
        if (advanced_media_player_) {
            delete advanced_media_player_;
            advanced_media_player_ = nullptr;
        }
    }
    void play(const std::string &audio_type, const std::string &file_name) override {
        if (audio_type == "vlc") {
            advanced_media_player_->play_vlc(file_name);
        } else if (audio_type == "mp4") {
            advanced_media_player_->play_mp4(file_name);
        }
    }
    void play_mp4(const std::string &file_name) {
        advanced_media_player_->play_mp4(file_name);
    }
    void play_vlc(const std::string &file_name) {
        advanced_media_player_->play_vlc(file_name);
    }

private:
    AdvancedMediaPlayer *advanced_media_player_;
};


class AudioPlayer : public MediaPlayer {
public:
    void play(const std::string &audio_type, const std::string &file_name) override {
        if (audio_type == "mp3") {
            std::cout << "Playing mp3 file. Name: " << file_name << std::endl;
        } else if (audio_type == "vlc" || audio_type == "mp4") {
            MediaAdapter *media_adapter = new MediaAdapter(audio_type);
            media_adapter->play(audio_type, file_name);
            delete media_adapter;
        } else {
            std::cout << "Invalid media. " << audio_type << " format not supported" << std::endl;
        }
    }
};
#endif

int main() {
    // 测试对象适配器
    MediaPlayer* audioPlayer = new AudioPlayer();
    audioPlayer->play("mp3", "beyond the horizon.mp3");
    audioPlayer->play("mp4", "alone.mp4");
    audioPlayer->play("vlc", "far far away.vlc");
    audioPlayer->play("avi", "mind me.avi");

    MediaAdapter* media_adapter = new  MediaAdapter("vlc");
    media_adapter->play("vlc", "far far away.vlc");
    media_adapter->play_vlc("example.vlc");

    delete audioPlayer;
    return 0;
}
#endif