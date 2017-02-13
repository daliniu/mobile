var Taf = Taf || {};
Taf.Util = Taf.Util || {};

Taf.Util.jcestream = function(value)
{
	if (value == null || value == undefined)
	{
		console.log("Taf.Util.jcestream::value is null or undefined");
		return ;
	}
	if (!(value instanceof ArrayBuffer))
	{
		console.log("Taf.Util.jcestream::value is not ArrayBuffer");
		return ;
	}

	var view = new Uint8Array(value);
	var str = "";
	for (var i = 0; i < view.length; i++)
	{
		if (i != 0 && i%16 == 0)
		{
			str += "\n";
		}
		else if (i!= 0)
		{
			str += " ";
		}

		str += (view[i] > 15?"":"0") + view[i].toString(16);
	}
	console.log(str.toUpperCase());
};
