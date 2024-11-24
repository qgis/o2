
#pragma once

#include <QWebEngineView>

class WebWindow;

class WebEnginePage : public QWebEnginePage
{
    Q_OBJECT

public:
	
    WebEnginePage(QWebEngineProfile *inProfile, const QString &inRedirectURLString) : QWebEnginePage(inProfile), mRedirectURLString(inRedirectURLString) { }
	
    ~WebEnginePage() override;

    bool acceptNavigationRequest(const QUrl & url, QWebEnginePage::NavigationType type, bool isMainFrame) override;
    
protected:
    QWebEnginePage *createWindow(WebWindowType type) override;

signals:
    void callbackCatched(const QString &inURLString);

private slots:
    void onCreatedWindowClosed();
    void onWindowCloseRequested();
	void onAuthWindowCallbackCalled(const QString &inURLString);

private:
	QString mRedirectURLString;
	std::vector<WebWindow *> mCreatedWindows;
};

