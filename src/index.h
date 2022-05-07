const char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<style type="text/css">
.button {
  background-color: #4CAF50; /* Green */
  border: none;
  color: white;
  padding: 15px 32px;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 16px;
}
</style>
<body style="background-color: #f9e79f ">
<center>
<div>
<h1>Udírna</h1>
<h3>AJAX BASED ESP8266 WEBSERVER</h1>
  <button class="button" onclick="send(1)">PLAMEN nedetekován</button>
  <!--<button class="button" onclick="send(0)">LED OFF</button>-->
  <BR>
</div>
 <br>
<div><h2>
  Teplota: <span id="adc_val">0</span> &deg;C<br><br>
  <!--LED State: <span id="state">NA</span>-->
</h2>
</div>
<script>
function send(led_sts) 
{
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("state").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "led_set?state="+led_sts, true);
  xhttp.send();
}
setInterval(function() 
{
  getData();
}, 2000); 
function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("adc_val").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "temperature", true);
  xhttp.send();
}
</script>
</center>
</body>
</html>
)=====";
