
var token=0;
var loop;
var timer=2000;
var timeout=timer+500;
var wait=0;

var FontSize_Extension = {
getDistance: function()
{	
	xmlHttp = new XMLHttpRequest();
	xmlHttp.open("GET", "http://127.0.0.1:9600?msg=<Req token="+token+"/>", true);
	xmlHttp.timeout=timeout;
	xmlHttp.setRequestHeader("Origin", "myDomain");

	xmlHttp.ontimeout=function()
	{	
		timer=timer+500;
		timeout=timeout+500;
	}
	
	xmlHttp.onload=function()
	{
		var doc = content.document;
		if(xmlHttp.readyState==4 && xmlHttp.status==200)
		{
			var xmlresp = $.parseXML(xmlHttp.responseText),
			
			$xmlresp = $( xmlresp ),
			
			$token = $xmlresp.find('Resp').attr('token');
			$dist = $xmlresp.find('distance');

			token=$token;
			token++;
			
			if($dist.text()==0)
				doc.body.setAttribute('style','');
			else{
				var zoomlv = 1;

				if($dist.text()>30)
					zoomlv = ((parseFloat($dist.text())-30)/100+1);
				doc.body.setAttribute('style','');
				doc.body.setAttribute('style','zoom:'+ zoomlv + ';-moz-transform: scale('+zoomlv+');  -moz-transform-origin: 0px 0px; ');
				}
		}

		if(xmlHttp.status==404)
		{
			window.alert("some proble occur durint http request");
			doc.body.setAttribute('style','');
		}	
	}

		xmlHttp.send(null);	
},

start: function()
{	
	if(token!=0){
		clearInterval(loop);
		token=0;
	}else{
		token=0;
		loop=setInterval(FontSize_Extension.getDistance, timer);
	}
}

//stop: function()
//{
//	clearInterval(loop);
//}
}
