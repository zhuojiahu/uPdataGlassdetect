#ifndef CARVE_SETTING_H
#define CARVE_SETTING_H

#include <QWidget>
#include <QStackedWidget>
#include <QToolButton>
#include "Widget_CarveImage.h"
#include "widget_image.h"
#include "cerrorimagelist.h"
#include <QButtonGroup>
#include "DHBaseWidget.h"

class myPushButton : public QPushButton
{
	Q_OBJECT

public:
	myPushButton(QWidget *parent = 0);
	~myPushButton();

};

class WidgetCarveSetting : public QWidget
{
	Q_OBJECT

public:
	WidgetCarveSetting(QWidget *parent = 0);
	~WidgetCarveSetting();
public slots:
	void slots_changeButtonMode();
	void slots_turnCameraPage(int index);
	void slots_showCarve();
	void slots_hideCarve();
public:
	QHBoxLayout *main_layout;
	QSplitter *splitter;

	ImageWidget *image_widget;//����ͼ����ʾ�����е�ͼ�����ģ��

	QWidget *setWidget;//ͼ����ʾ�����������н���
	QWidget *listWidget;
	Widget_CarveImage *widgetCarveImage;//��Ӧͼ����ʾ�������Ͻǵ��и�ͼƬ
	QStackedWidget *pStackedCarve;
	QButtonGroup *buttonGroupCamera;
	QToolButton *expandButton;
	CErrorImageList *errorList_widget;
	int iButtonMode;
	int iCameraNo;
	QList<Widget_CarveImage*> listWidgetCarveImage;

private:
// 	bool isShownCarveSet;
// 	bool isShownErrorList;
	QList<int> listSizeCarveSet;
	QList<int> listSplitter;

};

#endif // CARVE_SETTING_H