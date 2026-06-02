#ifndef VIDEOVIEW_H
#define VIDEOVIEW_H

#include <QWidget>

#include "util/VideoFrame.h"


class VideoView : public QWidget {
    Q_OBJECT
public:
    explicit VideoView(QWidget *parent = NULL);
    ~VideoView();

public slots:
    // 视频帧已被解码，准备展示到界面上
    void onFrameDecoded(VideoFramePtr videoFrame);

private:
    QImage *_image = NULL; // 图像控件
    QRect _rect; // 矩形框
    int count = 0;
    VideoFramePtr mVideoFrame; // 视频帧的指针
    void paintEvent(QPaintEvent *event) override;
    void freeImage(); // 释放图像资源
};

#endif // VIDEOVIEW_H
