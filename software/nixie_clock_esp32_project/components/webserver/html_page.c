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
static const char html_page[] = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Nixie Clock Control</title>
<style>
body {
  font-family: 'Roboto', sans-serif;
  background: #0d0d0d;
  color: #e0e0e0;
  margin: 0;
  display: flex;
  justify-content: center;
  align-items: flex-start;
  min-height: 100vh;
  padding: 50px 20px;
}
.card {
  background: #1e1e1e;
  border-radius: 16px;
  padding: 40px 30px;
  width: 380px;
  max-width: 100%%;
  box-shadow: 0 8px 25px rgba(0,0,0,0.7);
  border: 1px solid #2c2c2c;
}
h1 { font-size: 2.2em; margin-bottom: 10px; color: #ffffff; text-align: center; letter-spacing: 1px; }
h2 { font-size: 1.4em; margin: 25px 0 10px 0; color: #cccccc; border-bottom: 1px solid #333; padding-bottom: 5px; }
.input-row {
  display: flex;
  justify-content: center;
  align-items: center;
  margin-bottom: 15px;
}
.input-row input[type=number] {
  width: 60px;
  padding: 10px;
  margin: 0 5px;
  font-size: 18px;
  text-align: center;
  border-radius: 6px;
  border: 1px solid #333;
  background: #111;
  color: #fff;
  outline: none;
  transition: 0.2s;
}
.input-row input[type=number]:focus {
  border-color: #888;
  box-shadow: 0 0 5px #555;
}
.input-group {
  display: flex;
  align-items: center;
  margin-bottom: 10px;
}
.input-group label {
  width: 100px;
}
.input-group input[type=text] {
  flex: 1;
  padding: 10px;
  font-size: 16px;
  border-radius: 6px;
  border: 1px solid #333;
  background: #111;
  color: #fff;
  outline: none;
  box-sizing: border-box;
}
.input-group input[type=text]:focus {
  border-color: #888;
  box-shadow: 0 0 5px #555;
}
.checkbox-container { margin: 20px 0; }
.checkbox-container label {
  display: flex;
  align-items: center;
  font-size: 16px;
  margin-bottom: 10px;
  cursor: pointer;
  user-select: none;
}
.checkbox-container input[type=checkbox],
.checkbox-container input[type=radio] {
  width: 20px;
  height: 20px;
  margin-right: 12px;
}
button {
  width: 100%%;
  padding: 14px;
  font-size: 18px;
  background: #555;
  color: #fff;
  border: none;
  border-radius: 8px;
  cursor: pointer;
  transition: 0.25s;
  font-weight: bold;
}
button:hover { background: #777; }
hr { border: 0; border-top: 1px solid #333; margin: 20px 0; }
</style>
</head>
<body>
<div class="card">
<h1>Nixie Clock Control</h1>

<form action="/set_time" method="GET">

<h2>Set time</h2>
<div class="input-row">
  <input type="number" name="h" min="0" max="23" placeholder="HH" value="%d"> :
  <input type="number" name="m" min="0" max="59" placeholder="MM" value="%d"> :
  <input type="number" name="s" min="0" max="59" placeholder="SS" value="%d">
</div>

<h2>Wi-Fi</h2>
<div class="input-group">
  <label for="ssid">SSID:</label>
  <input type="text" id="ssid" name="ssid" value="%s">
</div>
<div class="input-group">
  <label for="password">Password:</label>
  <input type="text" id="password" name="password" value="%s">
</div>

<h2>Time synchronization</h2>
<div class="checkbox-container">
  <label><input type="checkbox" name="ntp" value="1" %s> Sync with NTP</label>
</div>

<h2>Mode</h2>
<div class="checkbox-container">
  <label><input type="radio" name="mode" value="0" %s> Hour mode</label>
  <label><input type="radio" name="mode" value="1" %s> Cathode antipoisoning mode</label>
  <label><input type="radio" name="mode" value="2" %s> Counter mode</label>
</div>

<button type="submit">Apply</button>
</form>
</div>
</body>
</html>
)rawliteral";

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/

/******************************************************************
 * 6. Functions definitions
******************************************************************/

/**
 * @brief Retourne le contenu HTML statique de la page.
 *
 * Cette fonction fournit un accès au tableau statique `html_page` contenant
 * le code HTML complet de la page. Le tableau est stocké dans le segment
 * en lecture seule et n'est pas modifiable par l'appelant.
 *
 * @return const char* Pointeur vers le début du contenu HTML.
 *
 * @note La mémoire est gérée statiquement ; il ne faut pas tenter de libérer
 *       le pointeur retourné.
 */
const char* get_html_page(void) {
    return html_page;
}
