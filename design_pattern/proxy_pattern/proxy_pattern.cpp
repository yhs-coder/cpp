#include <iostream>
#include <memory>

/*
 * 代理（proxy）模式: 代理类根据客户的等级/身份/关系，来控制对实际对象(委托类)的访问权限
 * 比如客户  助理（proxy代理类） 老板 （委托类）
 * 助理会根据客户的关系/身份/级别，决定是否开放访问老板的权限
 * */

// 视频网站
class VideoSite {
public:
    virtual void free_movie() = 0;  // 免费电影
    virtual void vip_movie() = 0;   // VIP电影
    virtual void ticket_movie() = 0;// 用券观看电影
};

// 委托类
class TXVideoSite : public VideoSite {
public:
    void free_movie() override {
        std::cout << "观看免费电影" << std::endl;
    }
    void vip_movie() override {
        std::cout << "观看vip电影" << std::endl;
    }
    void ticket_movie() override {
        std::cout << "用券观看电影" << std::endl;
    }
};

// 代理类 代理TXVideoSite
class FreeVideoSiteProxy : public VideoSite {
public:
    FreeVideoSiteProxy() {
        video_ = new TXVideoSite();
    }
    ~FreeVideoSiteProxy() {
        delete video_;
    }
    void free_movie() override {
        // 通过代理对象的free_movie，来访问真正的委托类对象的free_movie
        video_->free_movie();
    }
    void vip_movie() override {
        std::cout << "您目前是普通用户，需要升级为VIP，才能观看vip电影" << std::endl;
    }
    void ticket_movie() override {
        std::cout << "您目前没有券，需要购买电影券，才能观看电影" << std::endl;
    }

private:
    // 以组合的方式，使用委托对象的功能
    VideoSite *video_;
};

class VipVideoSiteProxy : public VideoSite {
public:
    VipVideoSiteProxy() {
        video_ = new TXVideoSite();
    }
    ~VipVideoSiteProxy() {
        delete video_;
    }
    void free_movie() override {
        // 通过代理对象的free_movie，来访问真正的委托类对象的free_movie
        video_->free_movie();
    }
    void vip_movie() override {
        video_->vip_movie();
    }
    void ticket_movie() override {
        std::cout << "您目前没有券，需要购买电影券，才能观看电影" << std::endl;
    }

private:
    VideoSite *video_;
};

void watch_movie(std::unique_ptr<VideoSite> &ptr) {
    ptr->free_movie();
    ptr->vip_movie();
    ptr->ticket_movie();
}

int main() {
    // 测试代理类
    // 不同身份的用户，通过访问代理对象，获得（委托类）不同的观看权限
    // 用户/客户直接访问代理对象
    std::unique_ptr<VideoSite> p1(new FreeVideoSiteProxy);
    watch_movie(p1);
    std::unique_ptr<VideoSite> p2(new VipVideoSiteProxy);
    watch_movie(p2);

    return 0;
}