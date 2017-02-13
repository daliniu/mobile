var Util = Util || {};

Util.str2ab = function (value)
{
    var idx, len = value.length, arr = new Array( len );
    for (idx = 0; idx < len; ++idx) 
    {
        arr[ idx ] = value.charCodeAt(idx & 255);
    }

    return new Uint8Array(arr).buffer;
}; 

Util.ajax = function (oData, oSuccFunc, oFailFunc) 
{
	var xmlobj = new XMLHttpRequest();
	xmlobj.overrideMimeType('text/plain; charset=x-user-defined'); 	//必须的&&一定要设置

	var handleStateChange = function () 
	{
		if (xmlobj.readyState === 4) 
		{
			if (xmlobj.status === 200 || xmlobj.status === 304) 
			{
				oSuccFunc(Util.str2ab(xmlobj.response));
			} 
			else 
			{
				oFailFunc(xmlobj.status);
			}

			xmlobj.removeEventListener('readystatechange', handleStateChange);
			xmlobj = undefined;
		}
	};

    xmlobj.addEventListener('readystatechange', handleStateChange);
    xmlobj.open("post", "http://10.12.22.10:8080?rand=" + Math.random());
	xmlobj.send(oData);
};
