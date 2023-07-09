#include "PageTemplate.h"

PageTemplate::PageTemplate(HttpHandler& server):_server(server) {
    _styles.push_back(R"(
        body {
          color: black;
          background-color: white;
          transition: color 0.3s ease-out, background-color 0.3s ease-out;
        }
        @media (prefers-color-scheme: dark) {
          body {
              color: #adaaaa;
              background-color: #151515;
          }
          ul {
              border-color: #3c3b3b;
              background: #181818;
          }
        }
        li {
            margin-top: 1em;
        }
        ul form {
            margin-bottom: 0em;
        }
        ul {
            border: 1px solid;
            border-color: #3c3b3b;
            padding-bottom: 0.5em;
            font-size: 0.85em;
        }
        ul.infoblock {
            list-style-type: none;
            padding-inline-start: 1em;
        }
    )");
}
void PageTemplate::addScript(String script){
    _scripts.push_back(script);
}
void PageTemplate::addStyle(String style){
    _styles.push_back(style);
}
void PageTemplate::addPrecontent(std::function<void(HttpHandler&)> renderPreContent){
    _precontents.push_back(renderPreContent);
}
void PageTemplate::setTitle(String title) {
    _title = title;
}
void PageTemplate::render(std::function<void(HttpHandler&)> renderContent) {
    _server.sendContent("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
    _server.sendContent("<html>");
    _server.sendContent("<meta charset=\"UTF-8\">");
    _server.sendContent("<meta name=\"color-scheme\" content=\"dark light\">");
    _server.sendContent(R"(
    <meta name="viewport" content="width=device-width, initial-scale=1">
    )"); 
    renderStyle();
    _server.sendContent("<body>\r\n");
    _server.sendContent("    <h1>");
    _server.sendContent(_title);
    _server.sendContent("</h1>\r\n");
    renderPrecontents();
    renderContent(_server);
    renderScript();
    
    _server.sendContent("\r\n</body></html>");
}
void PageTemplate::renderPrecontents() {
     for (auto precontent: _precontents) {
        precontent(_server);
        _server.sendContent("\r\n");
    }
}
void PageTemplate::renderStyle() {
    for (String style: _styles) {
        _server.sendContent("    <style>");
        _server.sendContent(style);
        _server.sendContent("</style>\r\n");
    }
}
void PageTemplate::renderScript() {
    for (String script: _scripts) {
        _server.sendContent("    <script type=\"text/javascript\">");
        _server.sendContent(script);
        _server.sendContent("</script>\r\n");
    }
}
