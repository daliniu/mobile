var Taf = Taf || {};

/**
 * KevinTian@tencent.com
 *
 * JCE编解码底层辅助类
 */
Taf.DataHelp =
{
    EN_INT8			:0,
    EN_INT16		:1,
    EN_INT32		:2,
    EN_INT64		:3,
    EN_FLOAT		:4,
    EN_DOUBLE		:5,
    EN_STRING1		:6,
    EN_STRING4		:7,
    EN_MAP			:8,
    EN_LIST			:9,
    EN_STRUCTBEGIN	:10,
    EN_STRUCTEND	:11,
    EN_ZERO			:12,
    EN_SIMPLELIST	:13
};

/**
 * 二进制Buffer类
 */
Taf.BinBuffer = function (buffer) 
{
    this.buf = null;
    this.uin = null;
    this.sin = null;
    this.len = 0;
    this.position = 0;

    if (buffer != null && buffer != undefined && buffer instanceof Taf.BinBuffer) 
    {
        this.buf = buffer.buf;
        this.uin = new Uint8Array(this.buf);
        this.sin = new Int8Array(this.buf);
        this.len = this.uin.length;
        this.position = buffer.position;
    }

    if (buffer != null && buffer != undefined && buffer instanceof ArrayBuffer)
    {
    	this.buf = buffer;
    	this.uin = new Uint8Array(this.buf);
    	this.sin = new Int8Array(this.buf);
    	this.len = this.uin.length;
    	this.position = 0;
    }

    this.__defineGetter__("length", function () { return this.len; });
    this.__defineGetter__("buffer", function () { return this.buf; });
};

Taf.BinBuffer.prototype.allocate = function (uiLength) 
{
    uiLength = this.position + uiLength;
    if (this.buf != null && this.buf.length > uiLength)
    {
        return;
    }

	var temp = new ArrayBuffer(Math.max(256, uiLength * 2));
    if (this.buf != null) 
    {
        new Uint8Array(temp).set(new Uint8Array(this.buf));
        this.buf = undefined;
    }

    this.buf = temp;
    this.uin = undefined;
    this.uin = new Uint8Array(this.buf);
    this.sin = new Int8Array(this.buf);
};

Taf.BinBuffer.prototype.getBuffer	= function()
{
	var temp = new ArrayBuffer(this.len);
	new Uint8Array(temp).set(new Uint8Array(this.buf, 0, this.len));
	return temp; 
}

Taf.BinBuffer.prototype.memset		= function(fbuf, offset, length)
{
	this.allocate(length);
	new Uint8Array(this.buf).set(new Uint8Array(fbuf, offset, length), this.position);
};

Taf.BinBuffer.prototype.writeInt8 	= function (value) 
{
    this.allocate(1);
    this.sin[this.position++] = value;
    this.len = this.position;
};

Taf.BinBuffer.prototype.writeUInt8 	= function (value) 
{
    this.allocate(1);
    this.uin[this.position++] = value;
    this.len = this.position;
};

Taf.BinBuffer.prototype.writeInt16 	= function (value) 
{
    this.allocate(2);
    this.uin[this.position++] = (value & 0x0000FF00) >> 8;
	this.uin[this.position++] = (value & 0x000000FF);
    this.len = this.position;
};

Taf.BinBuffer.prototype.writeUInt16 = function (value) 
{
    this.allocate(2);
    this.uin[this.position++] = (value & 0x0000FF00) >> 8;
	this.uin[this.position++] = (value & 0x000000FF);
    this.len = this.position;
};

Taf.BinBuffer.prototype.writeInt32 	= function (value) 
{
    this.allocate(4);
    this.uin[this.position++] = (value & 0xFF000000) >> 24; 
    this.uin[this.position++] = (value & 0x00FF0000) >> 16;
    this.uin[this.position++] = (value & 0x0000FF00) >>  8;
    this.uin[this.position++] = (value & 0x000000FF);
    this.len = this.position;
};

Taf.BinBuffer.prototype.writeUInt32 = function (value) 
{
	this.writeInt32(value);
};

Taf.BinBuffer.prototype.writeInt64 	= function (value) 
{
    this.allocate(8);

    var H4 = parseInt(value/4294967296);
    var L4 = value%4294967296;
	this.writeUInt32(H4);
	this.writeUInt32(L4);
};

Taf.BinBuffer.prototype.writeFloat 	= function (value) 
{
    this.allocate(4);
    this.uin.setFloat32(this.position, value);
    this.position += 4;
    this.len = this.position;
};

Taf.BinBuffer.prototype.writeDouble = function (value) 
{
    this.allocate(8);
    this.uin.setFloat64(this.position, value);
    this.position += 8;
    this.len = this.position;
};

Taf.BinBuffer.prototype.writeString = function (value) 
{
	for (var arr = [], i = 0; i < value.length; i++)
	{
		arr.push(value.charCodeAt(i) & 0xFF);
	}

    this.allocate(arr.length);
    new Uint8Array(this.buf).set(new Uint8Array(arr), this.position);
    this.position += arr.length;
    this.len = this.position;
};

Taf.BinBuffer.prototype.writeBytes	= function (value) 
{
	if (value.length == 0 || value.buf == null) return ;

	this.allocate(value.length);
	new Uint8Array(this.buf).set(new Uint8Array(value.buf, 0, value.length), this.position);
	this.position += value.length;
	this.len = this.position;
};

Taf.BinBuffer.prototype.readInt8 	= function () 
{
	return this.sin[this.position++];
};

Taf.BinBuffer.prototype.readInt16 	= function () 
{
	var temp0 = this.uin[this.position++];
	var temp1 = this.uin[this.position++];
    return ((temp0 << 8) | (temp1)) - (temp0 >= 0x80?65536:0);
};

Taf.BinBuffer.prototype.readInt32 = function () 
{
	var temp0 = this.uin[this.position++];
	var temp1 = this.uin[this.position++];
	var temp2 = this.uin[this.position++];
	var temp3 = this.uin[this.position++];
    return (temp0 << 24) | (temp1 << 16) | (temp2 << 8) | (temp3); 
};

Taf.BinBuffer.prototype.readUInt8  = function () 
{
    return this.uin[this.position++];
};

Taf.BinBuffer.prototype.readUInt16 = function () 
{
	var temp0 = this.uin[this.position++];
	var temp1 = this.uin[this.position++];
    return (temp0 << 8) | (temp1); 
};

Taf.BinBuffer.prototype.readUInt32 = function () 
{
    return this.readInt32() >>> 0;
};

Taf.BinBuffer.prototype.readInt64	= function()
{
	var H4 = this.readInt32();
	var L4 = this.readInt32();
	return (H4 * 4294967296 + L4) >>> 0;
};

Taf.BinBuffer.prototype.readString	= function (value) 
{
    for (var arr = [], i = 0; i < value; i++) 
    {
		arr.push(String.fromCharCode(this.uin[this.position++]));
    }

    return decodeURIComponent(escape(arr.join("")))
};

Taf.BinBuffer.prototype.readBytes	= function (value)
{
	var temp = new Taf.BinBuffer();
	temp.allocate(value);
	temp.memset(this.buf, this.position, value);
	temp.position = 0;
	temp.len = value;
	this.position = this.position + value;
	return temp;
};

/**
 * TAFJCE输出编解码包裹类
 *
 * KevinTian@tencent.com
 */
Taf.JceOutputStream = function () 
{
    this.buf = new Taf.BinBuffer();
    this.getBinBuffer = function () 
    {
        return this.buf;
    }
    this.getBuffer    = function () 
    {
        return this.buf.getBuffer();
    }
}

Taf.JceOutputStream.prototype.writeTo = function (tag, type) 
{
    if (tag < 15) 
    {
        this.buf.writeUInt8((tag << 4 & 0xF0) | type);
    }
    else
    {
        this.buf.writeUInt16((0xF0 | type) << 8 | tag);
    }
};

Taf.JceOutputStream.prototype.writeBoolean	= function (tag, value) 
{
    this.writeInt8(tag, value == true ? 1 : 0);
};

Taf.JceOutputStream.prototype.writeInt8 	= function (tag, value) 
{
	if (value == 0) 
    {
        this.writeTo(tag, Taf.DataHelp.EN_ZERO);
    }
    else 
    {
        this.writeTo(tag, Taf.DataHelp.EN_INT8);
        this.buf.writeInt8(value);
    }
};

Taf.JceOutputStream.prototype.writeInt16 = function (tag, value) 
{
    if (value >= -128 && value <= 127) 
    {
        this.writeInt8(tag, value);
    }
    else 
    {
        this.writeTo(tag, Taf.DataHelp.EN_INT16);
        this.buf.writeInt16(value);
    }
};

Taf.JceOutputStream.prototype.writeInt32 = function (tag, value) 
{
    if (value >= -32768 && value <= 32767) 
    {
        this.writeInt16(tag, value);
    }
    else 
    {
        this.writeTo(tag, Taf.DataHelp.EN_INT32);
        this.buf.writeInt32(value);
    }
};

Taf.JceOutputStream.prototype.writeInt64 = function (tag, value) 
{
    if (value >= -2147483648 && value <= 2147483647) 
    {
        this.writeInt32(tag, value);
    }
    else
    {
        this.writeTo(tag, Taf.DataHelp.EN_INT64);
        this.buf.writeInt64(value);
    }
};

Taf.JceOutputStream.prototype.writeUInt8 = function (tag, value) 
{
    this.writeInt16(tag, value);
}

Taf.JceOutputStream.prototype.writeUInt16 = function (tag, value) {
    this.writeInt32(tag, value);
}

Taf.JceOutputStream.prototype.writeUInt32 = function (tag, value) {
    this.writeInt64(tag, value);
}

Taf.JceOutputStream.prototype.writeStruct = function (tag, value) {
    if (value.writeTo == undefined) {
        throw Error("not defined writeTo Function");
    }

    this.writeTo(tag, Taf.DataHelp.EN_STRUCTBEGIN);
    value.writeTo(this);
    this.writeTo(0, Taf.DataHelp.EN_STRUCTEND);
}

Taf.JceOutputStream.prototype.writeString = function (tag, value) 
{
    var str = unescape(encodeURIComponent(value));
    if (str.length > 255) 
    {
        this.writeTo(tag, Taf.DataHelp.EN_STRING4);
        this.buf.writeUInt32(str.length);
    }
    else 
    {
        this.writeTo(tag, Taf.DataHelp.EN_STRING1);
        this.buf.writeUInt8(str.length);
    }

    this.buf.writeString(str);
};

Taf.JceOutputStream.prototype.writeBytes = function (tag, value)
{
    if (!(value instanceof Taf.BinBuffer)) 
    {
        throw Error("value not instanceof Taf.BinBuffer");
    }

    this.writeTo(tag, Taf.DataHelp.EN_SIMPLELIST);
    this.writeTo(0, Taf.DataHelp.EN_INT8);
    this.writeInt32(0, value.length);
    this.buf.writeBytes(value);
};

Taf.JceOutputStream.prototype.writeVector= function (tag, value)
{
	this.writeTo(tag, Taf.DataHelp.EN_LIST);
	this.writeInt32(0, value.val.length);
	for (var i = 0; i < value.val.length; i++)
	{
		value.proto._write(this, 0, value.val[i]);
	}
};

Taf.JceOutputStream.prototype.writeMap	= function (tag, value)
{
	this.writeTo(tag, Taf.DataHelp.EN_MAP);
	this.writeInt32(0, value.size());

	for(var temp in value.val) 
	{
		value.kproto._write(this, 0, temp);
		value.vproto._write(this, 1, value.val[temp]);
	}
};

/**
 * TAFJCE输入编解码包裹类
 *
 * KevinTian@tencent.com
 */
Taf.JceInputStream = function (buffer) 
{
    this.buf = new Taf.BinBuffer(buffer);
};

Taf.JceInputStream.prototype.readFrom = function () 
{
    var temp = this.buf.readUInt8();
    var tag  = (temp & 0xF0) >> 4;
    var type = (temp & 0x0F);

    if (tag >= 15) tag = this.buf.readUInt8();
    return {tag:tag, type:type};
};

Taf.JceInputStream.prototype.peekFrom = function () 
{
    var pos = this.buf.position;
    var head = this.readFrom();
    this.buf.position = pos;

    return {tag:head.tag, type:head.type, size:(head.tag >= 15) ? 2 : 1};
};

Taf.JceInputStream.prototype.skipField = function (type) 
{
    switch (type) 
    {
        case Taf.DataHelp.EN_INT8		: this.buf.position += 1; break;
        case Taf.DataHelp.EN_INT16    	: this.buf.position += 2; break;
        case Taf.DataHelp.EN_INT32    	: this.buf.position += 4; break;
        case Taf.DataHelp.EN_STRING1	: this.buf.position += this.buf.readUInt8(); break;
        case Taf.DataHelp.EN_STRING4	: this.buf.position += this.buf.readInt32(); break;
        case Taf.DataHelp.EN_STRUCTBEGIN: this.skipToStructEnd(); break;
        case Taf.DataHelp.EN_STRUCTEND	:
        case Taf.DataHelp.EN_ZERO		: break;
        case Taf.DataHelp.EN_MAP		:
		{
			var size = this.readInt32(0, true);
			
			for (var i = 0; i < size * 2; ++i)
			{
				var head = this.readFrom();
				this.skipField(head.type);
			}
			
			break;
		}
		case Taf.DataHelp.EN_SIMPLELIST	:
 		{
 			var head = this.readFrom();
 			if (head.type != Taf.DataHelp.EN_INT8)
 			{
 				throw Error("skipField with invalid type, type value: " + type + "," + head.type);
 			}

			this.buf.position += this.readInt32(0, true);
			break;
 		}
		case Taf.DataHelp.EN_LIST		:
 		{
 			var size = this.readInt32(0, true);
 			for (var i = 0; i < size; ++i)
 			{
 				var head = this.readFrom();
 				this.skipField(head.type);
			}
			break;
 		}
 		default                     	: throw new Error("skipField with invalid type, type value: " + type);
    }
}

Taf.JceInputStream.prototype.skipToStructEnd = function () 
{
    for (; ;) 
    {
        var head = this.readFrom();
        this.skipField(head.type);

        if (head.type == Taf.DataHelp.EN_STRUCTEND) 
        {
            return;
        }
    }
}

Taf.JceInputStream.prototype.skipToTag = function (tag, require) 
{
    while (this.buf.position < this.buf.length) 
    {
        var head = this.peekFrom();
        if (tag <= head.tag || head.type == Taf.DataHelp.EN_STRUCTEND) 
        {
        	if ((head.type == Taf.DataHelp.EN_STRUCTEND || tag != head.tag) && require)
        	{
				break;
        	}

            return true;
        }

        this.buf.position += head.size;
        this.skipField(head.type);
    }

	if (require) throw Error("require field not exist, tag:" + tag);
    return false;
}

Taf.JceInputStream.prototype.readBoolean = function (tag, require, def) {
    return this.readInt8(tag, require, def) == 1 ? true : false;
}

Taf.JceInputStream.prototype.readInt8 = function (tag, require, def) {
    if (this.skipToTag(tag, require) == false) { return def; }

    var head = this.readFrom();
    switch (head.type) 
    {
        case Taf.DataHelp.EN_ZERO: return 0;
        case Taf.DataHelp.EN_INT8: return this.buf.readInt8();
    }
    throw Error("read int8 type mismatch, tag:" + tag + ", get type:" + head.type);
}

Taf.JceInputStream.prototype.readInt16 = function (tag, require, def) {
	if (this.skipToTag(tag, require) == false) { return def; }

    var head = this.readFrom();
    switch (head.type) 
    {
        case Taf.DataHelp.EN_ZERO	: return 0;
        case Taf.DataHelp.EN_INT8	: return this.buf.readInt8();
        case Taf.DataHelp.EN_INT16	: return this.buf.readInt16();
    }
    throw Error("read int8 type mismatch, tag:" + tag + ", get type:" + head.type);
}

Taf.JceInputStream.prototype.readInt32 = function (tag, requrire, def) {
	if (this.skipToTag(tag, requrire) == false) { return def; }

    var head = this.readFrom();
    switch (head.type) 
    {
        case Taf.DataHelp.EN_ZERO  	: return 0;
        case Taf.DataHelp.EN_INT8	: return this.buf.readInt8();
        case Taf.DataHelp.EN_INT16  : return this.buf.readInt16();
        case Taf.DataHelp.EN_INT32	: return this.buf.readInt32();
    }
    throw Error("read int8 type mismatch, tag:" + tag + ", get type:" + head.type);
}

Taf.JceInputStream.prototype.readInt64 = function (tag, require, def) 
{
	if (this.skipToTag(tag, require) == false) { return def; }

    var head = this.readFrom();
    switch (head.type)
    {
    	case Taf.DataHelp.EN_ZERO	: return 0;
        case Taf.DataHelp.EN_INT8	: return this.buf.readInt8();
        case Taf.DataHelp.EN_INT16  : return this.buf.readInt16();
        case Taf.DataHelp.EN_INT32	: return this.buf.readInt32();
        case Taf.DataHelp.EN_INT64	: return this.buf.readInt64();
    }
    throw Error("read int64 type mismatch, tag:" + tag + ", get type:" + h.type)
}

Taf.JceInputStream.prototype.readUInt8 = function (tag, require, def) {
    return this.readInt16(tag, require, def);
}

Taf.JceInputStream.prototype.readUInt16 = function (tag, require, def) {
    return this.readInt32(tag, require, def);
}

Taf.JceInputStream.prototype.readUInt32 = function (tag, require, def) {
    return this.readInt64(tag, require, def);
}

Taf.JceInputStream.prototype.readStruct = function (tag, require, def) {
	if (this.skipToTag(tag, require) == false) { return def; }

    var head = this.readFrom();
    if (head.type != Taf.DataHelp.EN_STRUCTBEGIN) 
    {
        throw Error("read struct type mismatch, tag: " + tag + ", get type:" + head.type);
    }

    def.readFrom(this);
    this.skipToStructEnd();
    return def;
}

Taf.JceInputStream.prototype.readString = function (tag, require, def) {
	if (this.skipToTag(tag, require) == false) { return def; }

    var head = this.readFrom();
    if (head.type == Taf.DataHelp.EN_STRING1) {
        return this.buf.readString(this.buf.readUInt8());
    }

    if (head.type == Taf.DataHelp.EN_STRING4) {
        return this.buf.readString(this.buf.readUInt32());
    }

    throw Error("read 'string' type mismatch, tag: " + tag + ", get type: " + head.type + ".");
}

Taf.JceInputStream.prototype.readBytes	= function(tag, require, def)
{
	if (this.skipToTag(tag, require) == false) { return def; }

	var head = this.readFrom();
	if (head.type == Taf.DataHelp.EN_SIMPLELIST)
	{
		var temp = this.readFrom();
		if (temp.type != Taf.DataHelp.EN_INT8)
		{
			throw Error("type mismatch, tag:" + tag + ",type:" + head.type + "," + temp.type);
		}
		var size = this.readInt32(0, true);
		if (size < 0)
		{
			throw Error("invalid size, tag:" + tag + ",type:" + head.type + "," + temp.type);
		}              

		return this.buf.readBytes(size);
	}

	if (head.type == Taf.DataHelp.EN_LIST)
	{
		var size = this.readInt32(0, true);
		this.position += size;
		return new Taf.BinBuffer().memset(this.buf, this.position - size, size);				
	}

	throw Error("type mismatch, tag:" + tag + ",type:" + head.type);
}

Taf.JceInputStream.prototype.readVector	= function(tag, require, def)
{
	if (this.skipToTag(tag, require) == false) { return def; }

	var head = this.readFrom();
	if (head.type != Taf.DataHelp.EN_LIST)
	{
		throw Error("read 'vector' type mismatch, tag: " + tag + ", get type: " + head.type);
	}

	var size = this.readInt32(0, true);
	if (size < 0)
	{
		throw Error("invalid size, tag: " + tag + ", type: " + head.type + ", size: " + size);
	}
	for (var i = 0; i < size; ++i)
	{
		def.val.push(def.proto._read(this, 0, def.proto._clone()));
	}

	return def;
}

Taf.JceInputStream.prototype.readMap	= function(tag, require, def)
{
	if (this.skipToTag(tag, require) == false) { return def; }

	var head = this.readFrom();
	if (head.type != Taf.DataHelp.EN_MAP)
	{
		throw Error("read 'map' type mismatch, tag: " + tag + ", get type: " + head.type);
	}

	var size = this.readInt32(0, true);
	if (size < 0)
	{
		throw Error("invalid map, tag: " + tag + ", size: " + size);
	}

	for (var i = 0; i < size; i++)
	{
		var key = def.kproto._read(this, 0, def.kproto._clone());
		var val = def.vproto._read(this, 1, def.vproto._clone());
		def.put(key, val);
	}

	return def;
}
