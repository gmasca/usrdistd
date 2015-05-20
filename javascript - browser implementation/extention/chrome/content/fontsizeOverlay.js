

var FontSize_Extension = {

getDistance: function()
{		
	xmlHttp = new XMLHttpRequest();
		xmlHttp.open("GET", "http://127.0.0.1:9600", true);
		//xmlHttp.withCredentials = true
		//xmlHttp.timeout=5000;
		xmlHttp.setRequestHeader("Origin", "myDomain");
	
		xmlHttp.ontimeout=function()
		{
			FontSize_Extension.abort();
			window.alert("timeout expired for answer");
		}
		
		xmlHttp.onload=function()
		{
			var doc = content.document;

			if(xmlHttp.readyState==4 && xmlHttp.status==200)
			{
				var xmlresp = $.parseXML(xmlHttp.responseText),
				$xmlresp = $( xmlresp ),
				$dist = $xmlresp.find('distance');
				
				if($dist.text()==0)
					doc.body.setAttribute('style','');
				else{


					var zoomlv = 1;

					if($dist.text()>30)
						zoomlv = ((parseFloat($dist.text())-30)/100+1);
					//svar zoomlv = (parseFloat($dist.text())/100)+.5;
				
					doc.body.setAttribute('style','');
					doc.body.setAttribute('style','zoom:'+ zoomlv + ';-moz-transform: scale('+zoomlv+');  -moz-transform-origin: 0px 0px; ');
					}
			}

			if(xmlHttp.status==404)
			{
				window.alert("some proble occur durint http request");
				doc.body.setAttribute('style','');
				FontSize_Extension.reset();
			}
	
		}
		xmlHttp.send(null);	
},

reset: function()
{	

//s	console.log($(this))
	//$(this).each(function(index,elem){
//		console.log(elem);
	FontSize_Extension.getDistance()
	setTimeout(function(){}, 2000);
	FontSize_Extension.getDistance();
//	});
}
}
