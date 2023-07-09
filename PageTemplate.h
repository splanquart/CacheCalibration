#ifndef PAGE_TEMPLATE_H
#define PAGE_TEMPLATE_H

#include <vector>
#include "HttpHandler.h"

class PageTemplate {

public:
    PageTemplate(HttpHandler& _server);
    virtual void render(std::function<void(HttpHandler&)> renderContent);
    void addScript(String script);
    void addStyle(String style);
    void addPrecontent(std::function<void(HttpHandler&)> renderPreContent);
    void setTitle(String title);

protected:
    virtual void renderStyle();
    virtual void renderScript();
    virtual void renderPrecontents();

    std::vector<String> _styles;
    std::vector<String> _scripts;
    std::vector<std::function<void(HttpHandler&)>> _precontents;
    String _title;
    HttpHandler& _server;
};
#endif PAGE_TEMPLATE_H
