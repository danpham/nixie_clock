/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "html_page.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/
static const char html_page_data[] =
"<!DOCTYPE html>\n"
"<html lang=\"en\">\n"
"<head>\n"
"<meta charset=\"UTF-8\">\n"
"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
"<title>Nixie Clock Control</title>\n"
"<style>\n"
"body {\n"
"  font-family: 'Roboto', sans-serif;\n"
"  background: #0d0d0d;\n"
"  color: #e0e0e0;\n"
"  margin: 0;\n"
"  display: flex;\n"
"  justify-content: center;\n"
"  align-items: flex-start;\n"
"  min-height: 100vh;\n"
"  padding: 50px 20px;\n"
"}\n"
".card {\n"
"  background: #1e1e1e;\n"
"  border-radius: 16px;\n"
"  padding: 40px 30px;\n"
"  width: 380px;\n"
"  max-width: 100%%;\n"
"  box-shadow: 0 8px 25px rgba(0,0,0,0.7);\n"
"  border: 1px solid #2c2c2c;\n"
"}\n"
"h1 { font-size: 2.2em; margin-bottom: 10px; color: #ffffff; text-align: center; letter-spacing: 1px; }\n"
"h2 { font-size: 1.4em; margin: 25px 0 10px 0; color: #cccccc; border-bottom: 1px solid #333; padding-bottom: 5px; }\n"
".input-row {\n"
"  display: flex;\n"
"  justify-content: center;\n"
"  align-items: center;\n"
"  margin-bottom: 15px;\n"
"}\n"
".input-row input[type=number] {\n"
"  width: 60px;\n"
"  padding: 10px;\n"
"  margin: 0 5px;\n"
"  font-size: 18px;\n"
"  text-align: center;\n"
"  border-radius: 6px;\n"
"  border: 1px solid #333;\n"
"  background: #111;\n"
"  color: #fff;\n"
"  outline: none;\n"
"  transition: 0.2s;\n"
"}\n"
".input-row input[type=number]:focus { border-color: #888; box-shadow: 0 0 5px #555; }\n"
".input-group { display: flex; align-items: center; margin-bottom: 10px; }\n"
".input-group label { width: 100px; }\n"
".input-group input[type=text] {\n"
"  flex: 1;\n"
"  padding: 10px;\n"
"  font-size: 16px;\n"
"  border-radius: 6px;\n"
"  border: 1px solid #333;\n"
"  background: #111;\n"
"  color: #fff;\n"
"  outline: none;\n"
"  box-sizing: border-box;\n"
"}\n"
".input-group input[type=text]:focus { border-color: #888; box-shadow: 0 0 5px #555; }\n"
".checkbox-container { margin: 20px 0; }\n"
".checkbox-container label {\n"
"  display: flex;\n"
"  align-items: center;\n"
"  font-size: 16px;\n"
"  margin-bottom: 10px;\n"
"  cursor: pointer;\n"
"  user-select: none;\n"
"}\n"
".checkbox-container input[type=checkbox], .checkbox-container input[type=radio] {\n"
"  width: 20px;\n"
"  height: 20px;\n"
"  margin-right: 12px;\n"
"}\n"
"button {\n"
"  width: 100%%;\n"
"  padding: 14px;\n"
"  font-size: 18px;\n"
"  background: #555;\n"
"  color: #fff;\n"
"  border: none;\n"
"  border-radius: 8px;\n"
"  cursor: pointer;\n"
"  transition: 0.25s;\n"
"  font-weight: bold;\n"
"}\n"
"button:hover { background: #777; }\n"
"hr { border: 0; border-top: 1px solid #333; margin: 20px 0; }\n"
"</style>\n"
"</head>\n"
"<body>\n"
"<div class=\"card\">\n"
"<h1>Nixie Clock Control</h1>\n"
"<form action=\"/set_time\" method=\"GET\">\n"
"<h2>Set time</h2>\n"
"<div class=\"input-row\">\n"
"  <input type=\"number\" name=\"h\" min=\"0\" max=\"23\" placeholder=\"HH\" value=\"%d\"> :\n"
"  <input type=\"number\" name=\"m\" min=\"0\" max=\"59\" placeholder=\"MM\" value=\"%d\"> :\n"
"  <input type=\"number\" name=\"s\" min=\"0\" max=\"59\" placeholder=\"SS\" value=\"%d\">\n"
"</div>\n"
"<h2>Wi-Fi</h2>\n"
"<div class=\"input-group\">\n"
"  <label for=\"ssid\">SSID:</label>\n"
"  <input type=\"text\" id=\"ssid\" name=\"ssid\" value=\"%s\">\n"
"</div>\n"
"<div class=\"input-group\">\n"
"  <label for=\"password\">Password:</label>\n"
"  <input type=\"text\" id=\"password\" name=\"password\" value=\"%s\">\n"
"</div>\n"
"<h2>Time synchronization</h2>\n"
"<div class=\"checkbox-container\">\n"
"  <label><input type=\"checkbox\" name=\"ntp\" value=\"1\" %s> Sync with NTP</label>\n"
"</div>\n"
"<h2>Mode</h2>\n"
"<div class=\"checkbox-container\">\n"
"  <label><input type=\"radio\" name=\"mode\" value=\"0\" %s> Hour mode</label>\n"
"  <label><input type=\"radio\" name=\"mode\" value=\"1\" %s> Cathode antipoisoning mode</label>\n"
"  <label><input type=\"radio\" name=\"mode\" value=\"2\" %s> Counter mode</label>\n"
"</div>\n"
"<button type=\"submit\">Apply</button>\n"
"</form>\n"
"</div>\n"
"</body>\n"
"</html>\n";

static const char * const html_page = html_page_data;

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/

/******************************************************************
 * 6. Functions definitions
******************************************************************/

/**
 * @brief Returns the static HTML content of the page.
 *
 * This function provides access to the static `html_page` array containing
 * the full HTML code of the page. The array is stored in read-only memory
 * and cannot be modified by the caller.
 *
 * @return const char* Pointer to the beginning of the HTML content.
 *
 * @note The memory is managed statically; do not attempt to free
 *       the returned pointer.
 */
const char* get_html_page(void) {
    return html_page;
}
