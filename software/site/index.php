<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
        <title>Beagleway</title>
        <link href="./flot/examples/examples.css" rel="stylesheet" type="text/css">
        <link href="./beagleway.css" rel="stylesheet" type="text/css">
       <!--[if lte IE 8]><script language="javascript" type="text/javascript" src="../../excanvas.min.js"></script><![endif]-->
        <script language="javascript" type="text/javascript" src="./flot/jquery.js"></script>
        <script language="javascript" type="text/javascript" src="./flot/jquery.flot.js"></script>
        <script type="text/javascript">
            $(function() {

                // We use an inline data source in the example, usually data would
                // be fetched from a server

                var totalPoints = 100;
            
                function loadXMLDoc(cfunc)
                {
                    xmlhttp=new XMLHttpRequest();
                    xmlhttp.onreadystatechange=cfunc;
                    xmlhttp.open("GET","./getData.php?cant="+totalPoints,false);
                    xmlhttp.send();
                }
                
                function getData()
                {
                    var res = [];
                    var acelX = [];
                    var angleKalman = [];
                    var controlPID = [];
                    loadXMLDoc(function()
                    {
                        if (xmlhttp.readyState==4 && xmlhttp.status==200)
                        {
                            var results = xmlhttp.responseText.split('#');
                            
                            var acelXtmp = results[0].split('|');
                            var angleKalmantmp = results[1].split('|');
                            var controlPIDtmp = results[2].split('|');
                        
                            for (var i = 0; i < acelXtmp.length; ++i) {
                                acelX.push([i, acelXtmp[i]]);
                                angleKalman.push([i, angleKalmantmp[i]]);
                                controlPID.push([i, controlPIDtmp[i]]);
                            }
                            
                            res.push(acelX, angleKalman, controlPID);
                            
                            //                            document.getElementById("acelX").innerHTML=res[0];
                            //                            document.getElementById("angleKalman").innerHTML=res[1];
                            //                            document.getElementById("controlPID").innerHTML=res[2];
                        }
                    });
                    return res;
                }
                
                var data = [];

                function getRandomData() {

                    if (data.length > 0)
                        data = data.slice(1);

                    // Do a random walk

                    while (data.length < totalPoints) {

                        var prev = data.length > 0 ? data[data.length - 1] : 50,
                        y = prev + Math.random() * 1 - 0.5;

                        if (y < -1) {
                            y = -1;
                        } else if (y > 1) {
                            y = 1;
                        }

                        data.push(y);
                    }

                    // Zip the generated y values with the x values

                    var res = [];
                    for (var i = 0; i < data.length; ++i) {
                        res.push([i, data[i]])
                    }
                    document.getElementById("getRandomData").innerHTML=res;
                    return res;
                }

                // Set up the control widget

                var updateInterval = 500;
                $("#updateInterval").val(updateInterval).change(function () {
                    var v = $(this).val();
                    if (v && !isNaN(+v)) {
                        updateInterval = +v;
                        if (updateInterval < 1) {
                            updateInterval = 1;
                        } else if (updateInterval > 2000) {
                            updateInterval = 2000;
                        }
                        $(this).val("" + updateInterval);
                    }
                });
                
                function setParameter(param, value)
                {
                    var info = {
                        "param": param,
                        "value": value
                    }
            
                    $.ajax({
                        url:            "cgi-bin/setController.py",
                        type:           "POST",
                        data:           JSON.stringify(info),
                        contentType:    'application/json',
                        dataType:       "json",
                    }).done(function(response) {
                        document.getElementById(param+"Prueba").innerHTML=value;
                        alert(response.message);
                    }).fail(function(jqXHR, textStatus, errorThrown) {
                        alert(errorThrown);
                        alert(jqXHR.responseText);
                    });
                }
                
                var Kc = 50.0;
                $("#Kc").val(Kc).change(function () {
                    var v = $(this).val();
                    if (v && !isNaN(+v)) {
                        Kc = +v;
                        if (Kc < 0) {
                            Kc = 0;
                        } else if (Kc > 100) {
                            Kc = 100;
                        }
                        $(this).val("" + Kc);
                        
                        setParameter($(this).attr('id'), $(this).val());
                    }
                });
                
                var Ti = 0.5;
                $("#Ti").val(Ti).change(function () {
                    var v = $(this).val();
                    if (v && !isNaN(+v)) {
                        Ti = +v;
                        if (Ti < 0) {
                            Ti = 0;
                        } else if (Ti > 100) {
                            Ti = 100;
                        }
                        $(this).val("" + Ti);
                        
                        setParameter($(this).attr('id'), $(this).val());
                    }
                });
                
                var Td = 0.0;
                $("#Td").val(Td).change(function () {
                    var v = $(this).val();
                    if (v && !isNaN(+v)) {
                        Td = +v;
                        if (Td < 0) {
                            Td = 0;
                        } else if (Td > 100) {
                            Td = 100;
                        }
                        $(this).val("" + Td);
                        
                        setParameter($(this).attr('id'), $(this).val());
                    }
                });

                var data = getData();
                var plot = $.plot("#placeholder", [ 
                    { label: "Acel. X", data: data[0] },
                    { label: "Kalman", data: data[1] },
                    { label: "PID", data: data[2] }
                ], {
                    series: {
                        shadowSize: 0   // Drawing is faster without shadows
                    },
                    yaxis: {
                        ticks: 10,
                        min: -3,
                        max: 3,
                        tickDecimals: 3
                    },
                    xaxis: {
                        show: false
                    },
                    grid: {
                        backgroundColor: { colors: [ "#fff", "#eee" ] },
                        borderWidth: {
                            top: 1,
                            right: 1,
                            bottom: 2,
                            left: 2
                        }
                    }
                });

                function update() {
                    var data = getData();
                    plot.setData([data[0], data[1], data[2]]);
                        
                    // Since the axes don't change, we don't need to call plot.setupGrid()
                        
                    plot.draw();
                    setTimeout(update, updateInterval);
                }
                    
                update();

                // Add the Flot version string to the footer

                //$("#footer").prepend("Flot " + $.plot.version + " &ndash; ");
                
            });

        </script>
    </head>
    <body>

        <div id="header">
            <h2>Beagleway web control page</h2>
        </div>

        <div id="content">

            <div class="demo-container">
                <div id="placeholder" class="demo-placeholder"></div>
            </div>

            <p><b>Configuración de Parámetros:</b></p>

            <table>
                <tr>
                    <td>
                        Sample time:
                    </td>
                    <td>
                        <input id="updateInterval" type="text" value="" style="text-align: right; width:5em"> milliseconds
                    </td>
                </tr>

                <tr>
                    <td>
                        Kc (P):
                    </td>
                    <td>
                        <input id="Kc" type="text" value="" class="inputParameter" data-inputParameterId="Kc">
                    </td>
                    <td>
                        <span id="KcPrueba"></span>
                    </td>
                </tr>

                <tr>
                    <td>
                        TI (I):
                    </td>
                    <td>
                        <input id="Ti" type="text" value="" class="inputParameter" data-inputParameterId="Ti">
                    </td>
                    <td>
                        <span id="TiPrueba"></span>
                    </td>
                </tr>

                <tr>
                    <td>
                        Td (D):
                    </td>
                    <td>
                        <input id="Td" type="text" value="" class="inputParameter" data-inputParameterId="Td">
                    </td>
                    <td>
                        <span id="TdPrueba"></span>
                    </td>
                </tr>
            </table>

            <!--<p>Random: <span id="getRandomData"></span></p>-->
<!--            <p>Acel. X: <span id="acelX"></span></p>
            <p>Angle Kalman: <span id="angleKalman"></span></p>
            <p>PID: <span id="controlPID"></span></p>-->

        </div>

        <div id="footer">
            Copyright &copy; 2013 Gary Servin - Luis Sandoval
        </div>

    </body>
</html>
