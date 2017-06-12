<!DOCTYPE html>
<html lang="es">
<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<title>Piso APSD</title>
  <link rel="stylesheet" type="text/css" href="node_modules/jquery-ui/themes/base/all.css"> 
  <link rel="stylesheet" type="text/css" href="css/estilo.css">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
</head>
<body>
<div id="piso">
  <h1>Pis d'APSD</h1>
  <div class="row">
    <div class="col-8 mov">
      <fieldset id="mov">
        <legend>Detector de movimiento</legend>
        <p id="res"></p>
              <br><ul>
                <li id="entrada-mov">Entrada</li>
                <li id="coci-techo-mov">Cocina</li>
                <li id="comedor-mov">Comedor</li>
                <li id="aseo-mov">Aseo</li>
                <li id="dorm-mov">Dormitorio</li>
                <li id="trast-mov">Trastero</li>
              </ul>
      </fieldset>
      <!-- Pestañas -->
<div id="tabs">
  <ul>
    <li><a href="#tab-entrada">Entrada</a></li>
    <li><a href="#tab-cocina">Cocina</a></li>    
    <li><a href="#tab-comedor">Comedor</a></li>        
    <li><a href="#tab-aseo">Aseo</a></li>    
    <li><a href="#tab-dormitorio">Dormitorio</a></li>      
    <li><a href="#tab-trastero">Trastero</a></li>
  </ul>
  <div id="tab-entrada">
    <div class="row">
      <div class="col-6 menu">    
        <fieldset id="authentication_method">
          <legend>M&eacute;todos de autenticaci&oacute;n</legend>
            <input type="radio" name="autenticacion" id="entrada-teclado" value="Teclado">
            <label id="lblTeclado" valign="middle">Teclado </label><span id="valor-teclado"></span><br>
            <input type="radio" name="autenticacion" id="entrada-huella" value="Huella">
            <label id="lblHuella" valign="middle">Huella </label><span id="valor-huella"></span><br>
            <input type="radio" name="autenticacion" id="entrada-RFID" value="RFID">
            <label id="lblRFID" valign="middle">RFID </label><span id="valor-RFID"></span><br>
            <button id="btn-ent-abrir">Abrir puerta</button>
        </fieldset>
      <div id="fondo-img" class="rojo"><img id="img-puerta" class="tamanyo" src="images/enter.png" /></div>
      </div>
      
  </div>
  </div>
  <div id="tab-cocina">
    <div class="row">
      <div class="col-6">
        <fieldset id="cocina-fregadero">
          <legend>Grifo</legend>
        <ul>
          <p><li>Temperatura <input size="3" type="text" id="coci-freg-temp"></li></p>
          <p><li><span id="coci-freg-humedad">Humedad</span></li></p>
          <p><li>Caudal <input size="3" type="text" id="coci-freg-caudal"></li></p>
        </ul>            
        <span class="cool_checkbox"><input type="checkbox" class="btn-auto" id="ent-auto-1"/><label for="ent-auto-1"><span>Auto</span><span><span>/</span></span><span>Manual</span></label></span>

        <span class="cool_checkbox"><input type="checkbox" id="coci-freg-grifo"/><label for="ent-on-grifo"><span>On</span><span><span>/</span></span><span>Off</span></label></span>
        <img class="tamanyo" src="images/grifo-off.png" />
      </fieldset>
    </div>
      <div class="col-6">
        <fieldset id="cocina-techo">
          <legend>Sensores cocina</legend>

        <ul>
          <p><li><label for="coci-techo-temp">Temperatura </label><input size="3" type="text" id="coci-techo-temp"></li></p>
          <p><li><span id="coci-techo-llama">Llama</span></li></p>
            <p><li><label for="coci-techo-gas">Gas </label><input size="3" type="text" id="coci-techo-gas"></li></p>
      </ul>
      </fieldset>
    </div>
    </div>
  </div>
  <div id="tab-comedor">
   <div class="row">
      <div class="col-6 menu">
        <fieldset id="comedor">
          <legend>Sensores del comedor</legend>
        <ul>
          <p><li>Temperatura <input size="3" type="text" id="comedor-temp"></li></p>
      </ul>
        <img id="persiana" class="ttamanyo" src="images/window-1.png" />
      <div class="widget">
        <button value="0">Subir hasta arriba</button>
        <button value="1">Subir</button>
        <button value="3">Bajar</button>
        <button value="4">Bajar hasta abajo</button>
      </div>
      </fieldset>
    </div>
   </div>
  </div>
  <div id="tab-aseo">
   <div class="row">
      <div class="col-6 menu">
        <fieldset id="aseo">
          <legend>Sensores del aseo</legend>
            <ul>
              <p><li>Temperatura <input size="3" type="text" id="aseo-temp"></li></p>
              <p><li><span id="aseo-humedad">Humedad</span></li></p>
            </ul>
        <label title="Información del caudal de humedad" id="lblInfoHumedadAseo" valign="middle">Info</label><br>
        <label id="lblTiempoRecepcionHumedadAseo" valign="middle">Tiempo de recepci&oacute;n</label><br>
        <label id="lblTiempoMaximoHumedadAseo" valign="middle">Tiempo m&aacute;ximo de humedad</label><br>            

        <span class="cool_checkbox"><input type="checkbox" id="ent-auto-electrov-aseo-1"/><label for="ent-auto-electrov-aseo-1"><span>Auto</span><span><span>/</span></span><span>Manual</span></label></span>

        <span class="cool_checkbox"><input type="checkbox" id="ent-on-electrov-aseo"/><label for="ent-on-electrov-aseo"><span>On</span><span><span>/</span></span><span>Off</span></label></span>
        <img class="tamanyo" src="images/grifo-off.png" />
      </fieldset>
    </div>
   </div>
  </div>  
  <div id="tab-dormitorio">
   <div class="row">
      <div class="col-6 menu">
        <fieldset id="dormitorio">
          <legend>Dormitorio</legend>
        <ul>
          <p><li>Temperatura <input size="3" type="text" id="dorm-temp"></li></p>
      </ul>
      </fieldset>
    </div>
   </div>
  </div>
  <div id="tab-trastero">
   <div class="row">
      <div class="col-6 menu">
        <fieldset id="trastero">
          <legend>Sensores del trastero</legend>
        <ul>
      </ul>
      </fieldset>
    </div>
   </div>

  </div>
  
</div>

      <!-- Fin Pestañas -->
    </div>
    <div class="col-4 mov">
      <fieldset id="general">
        <legend>Alarma</legend>
        <span class="cool_checkbox"><input type="checkbox" id="ent-auto-alarma" class="btn-auto" /><label for="ent-auto-alarma"><span>Man</span><span><span>/</span></span><span>Auto</span></label></span>
        <span class="cool_checkbox"><input type="checkbox" id="alarma" class="btn-on" /><label for="alarma"><span>On</span><span><span>/</span></span><span>Off</span></label></span>
        <img id="img-alarma" class="tamanyo" src="images/luz-off.png" />
        <button id="btn-reset-alarma">Resetear Alarma</button>
        <p id="txt-alarma"></p>
      </fieldset>
    </div>
  </div>
</div>


<!--
<script   src="https://code.jquery.com/jquery-3.2.1.min.js"   integrity="sha256-hwg4gsxgFZhOsEEamdOYGBf13FyQuiTwlAQgxVSNgt4="   crossorigin="anonymous"></script> 
<script   src="https://code.jquery.com/ui/1.12.1/jquery-ui.min.js"   integrity="sha256-VazP97ZCwtekAsvgPBSUwPFKdrwD3unUfSGVYrahUqU="   crossorigin="anonymous"></script>
-->
<script type="text/javascript" src="node_modules/jquery/dist/jquery.min.js"></script>
<script type="text/javascript" src="scripts/lib/jquery-ui.min.js"></script>
<script type="text/javascript" src="scripts/sensores.json"></script>
<script type="text/javascript" src="scripts/index.js"></script>
</body>
</html>
