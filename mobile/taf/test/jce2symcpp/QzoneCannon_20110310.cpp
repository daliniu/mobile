#include "./QzoneCannon_20110310.h"


namespace cannon
{
    EXPORT_C taf::String Profile::className()
    {
        return taf::String((const signed char *)"cannon.Profile");
    }

    EXPORT_C taf::String Profile::structName() const
    {
        return taf::String((const signed char *)"cannon.Profile");
    }

    EXPORT_C Profile::Profile()
    : uin(0),nickname(taf::String((const signed char *)"")),qzonename(taf::String((const signed char *)"")),vip(0),gender(0),age(0),birthday(0),astro(0),marriage(0),country(taf::String((const signed char *)"")),province(taf::String((const signed char *)"")),city(taf::String((const signed char *)"")),Portrait(taf::String((const signed char *)"")),birthdaystr(taf::String((const signed char *)""))
    {
    }

    EXPORT_C void Profile::reset()
    {
        uin = 0;
        nickname = taf::String((const signed char *)"");
        qzonename = taf::String((const signed char *)"");
        vip = 0;
        gender = 0;
        age = 0;
        birthday = 0;
        astro = 0;
        marriage = 0;
        country = taf::String((const signed char *)"");
        province = taf::String((const signed char *)"");
        city = taf::String((const signed char *)"");
        Portrait = taf::String((const signed char *)"");
        birthdaystr = taf::String((const signed char *)"");
    }

    EXPORT_C void Profile::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(uin, 1);
        _os.write(nickname, 2);
        _os.write(qzonename, 3);
        _os.write(vip, 4);
        if (gender != 0)
        {
            _os.write(gender, 5);
        }
        if (age != 0)
        {
            _os.write(age, 6);
        }
        if (birthday != 0)
        {
            _os.write(birthday, 7);
        }
        if (astro != 0)
        {
            _os.write(astro, 8);
        }
        if (marriage != 0)
        {
            _os.write(marriage, 9);
        }
        if (!(country == taf::String((const signed char *)"")))
        {
            _os.write(country, 10);
        }
        if (!(province == taf::String((const signed char *)"")))
        {
            _os.write(province, 11);
        }
        if (!(city == taf::String((const signed char *)"")))
        {
            _os.write(city, 12);
        }
        if (!(Portrait == taf::String((const signed char *)"")))
        {
            _os.write(Portrait, 13);
        }
        if (!(birthdaystr == taf::String((const signed char *)"")))
        {
            _os.write(birthdaystr, 14);
        }
    }

    EXPORT_C void Profile::readFrom(taf::JceInputStream& _is)
    {
        _is.read(uin, 1, true);
        _is.read(nickname, 2, true);
        _is.read(qzonename, 3, true);
        _is.read(vip, 4, true);
        _is.read(gender, 5, false);
        _is.read(age, 6, false);
        _is.read(birthday, 7, false);
        _is.read(astro, 8, false);
        _is.read(marriage, 9, false);
        _is.read(country, 10, false);
        _is.read(province, 11, false);
        _is.read(city, 12, false);
        _is.read(Portrait, 13, false);
        _is.read(birthdaystr, 14, false);
    }

    EXPORT_C Profile& Profile::operator=(const Profile& r)
    {
        this->uin = r.uin;
        this->nickname = r.nickname;
        this->qzonename = r.qzonename;
        this->vip = r.vip;
        this->gender = r.gender;
        this->age = r.age;
        this->birthday = r.birthday;
        this->astro = r.astro;
        this->marriage = r.marriage;
        this->country = r.country;
        this->province = r.province;
        this->city = r.city;
        this->Portrait = r.Portrait;
        this->birthdaystr = r.birthdaystr;
        return *this;
    }

    EXPORT_C bool operator==(const Profile& l, const Profile& r)
    {
        return l.uin == r.uin && l.nickname == r.nickname && l.qzonename == r.qzonename && l.vip == r.vip && l.gender == r.gender && l.age == r.age && l.birthday == r.birthday && l.astro == r.astro && l.marriage == r.marriage && l.country == r.country && l.province == r.province && l.city == r.city && l.Portrait == r.Portrait && l.birthdaystr == r.birthdaystr;
    }

    EXPORT_C bool operator!=(const Profile&l, const Profile&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String BlogTitle::className()
    {
        return taf::String((const signed char *)"cannon.BlogTitle");
    }

    EXPORT_C taf::String BlogTitle::structName() const
    {
        return taf::String((const signed char *)"cannon.BlogTitle");
    }

    EXPORT_C BlogTitle::BlogTitle()
    : uin(0),blogid(0),title(taf::String((const signed char *)"")),read(0),comment(0),effect(0),pubdate(0),category(taf::String((const signed char *)"")),content(taf::String((const signed char *)"")),rights(0)
    {
    }

    EXPORT_C void BlogTitle::reset()
    {
        uin = 0;
        blogid = 0;
        title = taf::String((const signed char *)"");
        read = 0;
        comment = 0;
        effect = 0;
        pubdate = 0;
        category = taf::String((const signed char *)"");
        content = taf::String((const signed char *)"");
        rights = 0;
    }

    EXPORT_C void BlogTitle::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(uin, 1);
        _os.write(blogid, 2);
        _os.write(title, 3);
        if (read != 0)
        {
            _os.write(read, 4);
        }
        if (comment != 0)
        {
            _os.write(comment, 5);
        }
        if (effect != 0)
        {
            _os.write(effect, 6);
        }
        if (pubdate != 0)
        {
            _os.write(pubdate, 7);
        }
        if (!(category == taf::String((const signed char *)"")))
        {
            _os.write(category, 8);
        }
        if (!(content == taf::String((const signed char *)"")))
        {
            _os.write(content, 9);
        }
        if (rights != 0)
        {
            _os.write(rights, 10);
        }
    }

    EXPORT_C void BlogTitle::readFrom(taf::JceInputStream& _is)
    {
        _is.read(uin, 1, true);
        _is.read(blogid, 2, true);
        _is.read(title, 3, true);
        _is.read(read, 4, false);
        _is.read(comment, 5, false);
        _is.read(effect, 6, false);
        _is.read(pubdate, 7, false);
        _is.read(category, 8, false);
        _is.read(content, 9, false);
        _is.read(rights, 10, false);
    }

    EXPORT_C BlogTitle& BlogTitle::operator=(const BlogTitle& r)
    {
        this->uin = r.uin;
        this->blogid = r.blogid;
        this->title = r.title;
        this->read = r.read;
        this->comment = r.comment;
        this->effect = r.effect;
        this->pubdate = r.pubdate;
        this->category = r.category;
        this->content = r.content;
        this->rights = r.rights;
        return *this;
    }

    EXPORT_C bool operator==(const BlogTitle& l, const BlogTitle& r)
    {
        return l.uin == r.uin && l.blogid == r.blogid && l.title == r.title && l.read == r.read && l.comment == r.comment && l.effect == r.effect && l.pubdate == r.pubdate && l.category == r.category && l.content == r.content && l.rights == r.rights;
    }

    EXPORT_C bool operator!=(const BlogTitle&l, const BlogTitle&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String BlogInfo::className()
    {
        return taf::String((const signed char *)"cannon.BlogInfo");
    }

    EXPORT_C taf::String BlogInfo::structName() const
    {
        return taf::String((const signed char *)"cannon.BlogInfo");
    }

    EXPORT_C BlogInfo::BlogInfo()
    : uin(0),blogid(0),title(taf::String((const signed char *)"")),content(taf::String((const signed char *)"")),sign(taf::String((const signed char *)"")),cat(taf::String((const signed char *)"")),pubdate(0),effect(0),read(0),comment(0),authorid(0),authorname(taf::String((const signed char *)""))
    {
    }

    EXPORT_C void BlogInfo::reset()
    {
        uin = 0;
        blogid = 0;
        title = taf::String((const signed char *)"");
        content = taf::String((const signed char *)"");
        sign = taf::String((const signed char *)"");
        cat = taf::String((const signed char *)"");
        pubdate = 0;
        effect = 0;
        read = 0;
        comment = 0;
        authorid = 0;
        authorname = taf::String((const signed char *)"");
    }

    EXPORT_C void BlogInfo::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(uin, 1);
        _os.write(blogid, 2);
        _os.write(title, 3);
        _os.write(content, 4);
        _os.write(sign, 5);
        _os.write(cat, 6);
        _os.write(pubdate, 7);
        if (effect != 0)
        {
            _os.write(effect, 8);
        }
        if (read != 0)
        {
            _os.write(read, 9);
        }
        if (comment != 0)
        {
            _os.write(comment, 10);
        }
        if (authorid != 0)
        {
            _os.write(authorid, 11);
        }
        if (!(authorname == taf::String((const signed char *)"")))
        {
            _os.write(authorname, 12);
        }
    }

    EXPORT_C void BlogInfo::readFrom(taf::JceInputStream& _is)
    {
        _is.read(uin, 1, true);
        _is.read(blogid, 2, true);
        _is.read(title, 3, true);
        _is.read(content, 4, true);
        _is.read(sign, 5, true);
        _is.read(cat, 6, true);
        _is.read(pubdate, 7, true);
        _is.read(effect, 8, false);
        _is.read(read, 9, false);
        _is.read(comment, 10, false);
        _is.read(authorid, 11, false);
        _is.read(authorname, 12, false);
    }

    EXPORT_C BlogInfo& BlogInfo::operator=(const BlogInfo& r)
    {
        this->uin = r.uin;
        this->blogid = r.blogid;
        this->title = r.title;
        this->content = r.content;
        this->sign = r.sign;
        this->cat = r.cat;
        this->pubdate = r.pubdate;
        this->effect = r.effect;
        this->read = r.read;
        this->comment = r.comment;
        this->authorid = r.authorid;
        this->authorname = r.authorname;
        return *this;
    }

    EXPORT_C bool operator==(const BlogInfo& l, const BlogInfo& r)
    {
        return l.uin == r.uin && l.blogid == r.blogid && l.title == r.title && l.content == r.content && l.sign == r.sign && l.cat == r.cat && l.pubdate == r.pubdate && l.effect == r.effect && l.read == r.read && l.comment == r.comment && l.authorid == r.authorid && l.authorname == r.authorname;
    }

    EXPORT_C bool operator!=(const BlogInfo&l, const BlogInfo&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String CommentReply::className()
    {
        return taf::String((const signed char *)"cannon.CommentReply");
    }

    EXPORT_C taf::String CommentReply::structName() const
    {
        return taf::String((const signed char *)"cannon.CommentReply");
    }

    EXPORT_C CommentReply::CommentReply()
    : uin(0),nickname(taf::String((const signed char *)"")),content(taf::String((const signed char *)"")),pubdate(0)
    {
    }

    EXPORT_C void CommentReply::reset()
    {
        uin = 0;
        nickname = taf::String((const signed char *)"");
        content = taf::String((const signed char *)"");
        pubdate = 0;
    }

    EXPORT_C void CommentReply::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(uin, 1);
        _os.write(nickname, 2);
        _os.write(content, 3);
        if (pubdate != 0)
        {
            _os.write(pubdate, 4);
        }
    }

    EXPORT_C void CommentReply::readFrom(taf::JceInputStream& _is)
    {
        _is.read(uin, 1, true);
        _is.read(nickname, 2, true);
        _is.read(content, 3, true);
        _is.read(pubdate, 4, false);
    }

    EXPORT_C CommentReply& CommentReply::operator=(const CommentReply& r)
    {
        this->uin = r.uin;
        this->nickname = r.nickname;
        this->content = r.content;
        this->pubdate = r.pubdate;
        return *this;
    }

    EXPORT_C bool operator==(const CommentReply& l, const CommentReply& r)
    {
        return l.uin == r.uin && l.nickname == r.nickname && l.content == r.content && l.pubdate == r.pubdate;
    }

    EXPORT_C bool operator!=(const CommentReply&l, const CommentReply&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String BlogComment::className()
    {
        return taf::String((const signed char *)"cannon.BlogComment");
    }

    EXPORT_C taf::String BlogComment::structName() const
    {
        return taf::String((const signed char *)"cannon.BlogComment");
    }

    EXPORT_C BlogComment::BlogComment()
    : blogid(0),cmtid(0),userid(0),username(taf::String((const signed char *)"")),content(taf::String((const signed char *)"")),pubdate(0),sign(taf::String((const signed char *)"")),archive(0),effect(0),replyCount(0),replistsize(0)
    {
    }

    EXPORT_C void BlogComment::reset()
    {
        blogid = 0;
        cmtid = 0;
        userid = 0;
        username = taf::String((const signed char *)"");
        content = taf::String((const signed char *)"");
        pubdate = 0;
        sign = taf::String((const signed char *)"");
        archive = 0;
        effect = 0;
        replylist.clear();
        replyCount = 0;
        replistsize = 0;
    }

    EXPORT_C void BlogComment::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(blogid, 1);
        _os.write(cmtid, 2);
        _os.write(userid, 3);
        _os.write(username, 4);
        _os.write(content, 5);
        _os.write(pubdate, 6);
        if (!(sign == taf::String((const signed char *)"")))
        {
            _os.write(sign, 7);
        }
        if (archive != 0)
        {
            _os.write(archive, 8);
        }
        if (effect != 0)
        {
            _os.write(effect, 9);
        }
        if (replylist.size() > 0)
        {
            _os.write(replylist, 10);
        }
        if (replyCount != 0)
        {
            _os.write(replyCount, 11);
        }
        if (replistsize != 0)
        {
            _os.write(replistsize, 12);
        }
    }

    EXPORT_C void BlogComment::readFrom(taf::JceInputStream& _is)
    {
        _is.read(blogid, 1, true);
        _is.read(cmtid, 2, true);
        _is.read(userid, 3, true);
        _is.read(username, 4, true);
        _is.read(content, 5, true);
        _is.read(pubdate, 6, true);
        _is.read(sign, 7, false);
        _is.read(archive, 8, false);
        _is.read(effect, 9, false);
        _is.read(replylist, 10, false);
        _is.read(replyCount, 11, false);
        _is.read(replistsize, 12, false);
    }

    EXPORT_C BlogComment& BlogComment::operator=(const BlogComment& r)
    {
        this->blogid = r.blogid;
        this->cmtid = r.cmtid;
        this->userid = r.userid;
        this->username = r.username;
        this->content = r.content;
        this->pubdate = r.pubdate;
        this->sign = r.sign;
        this->archive = r.archive;
        this->effect = r.effect;
        this->replylist = r.replylist;
        this->replyCount = r.replyCount;
        this->replistsize = r.replistsize;
        return *this;
    }

    EXPORT_C bool operator==(const BlogComment& l, const BlogComment& r)
    {
        return l.blogid == r.blogid && l.cmtid == r.cmtid && l.userid == r.userid && l.username == r.username && l.content == r.content && l.pubdate == r.pubdate && l.sign == r.sign && l.archive == r.archive && l.effect == r.effect && l.replylist == r.replylist && l.replyCount == r.replyCount && l.replistsize == r.replistsize;
    }

    EXPORT_C bool operator!=(const BlogComment&l, const BlogComment&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String Visitor::className()
    {
        return taf::String((const signed char *)"cannon.Visitor");
    }

    EXPORT_C taf::String Visitor::structName() const
    {
        return taf::String((const signed char *)"cannon.Visitor");
    }

    EXPORT_C Visitor::Visitor()
    : uin(0),name(taf::String((const signed char *)"")),visittime(0),portrait(taf::String((const signed char *)"")),lastmood(taf::String((const signed char *)""))
    {
    }

    EXPORT_C void Visitor::reset()
    {
        uin = 0;
        name = taf::String((const signed char *)"");
        visittime = 0;
        portrait = taf::String((const signed char *)"");
        lastmood = taf::String((const signed char *)"");
    }

    EXPORT_C void Visitor::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(uin, 1);
        _os.write(name, 2);
        if (visittime != 0)
        {
            _os.write(visittime, 3);
        }
        if (!(portrait == taf::String((const signed char *)"")))
        {
            _os.write(portrait, 4);
        }
        if (!(lastmood == taf::String((const signed char *)"")))
        {
            _os.write(lastmood, 5);
        }
    }

    EXPORT_C void Visitor::readFrom(taf::JceInputStream& _is)
    {
        _is.read(uin, 1, true);
        _is.read(name, 2, true);
        _is.read(visittime, 3, false);
        _is.read(portrait, 4, false);
        _is.read(lastmood, 5, false);
    }

    EXPORT_C Visitor& Visitor::operator=(const Visitor& r)
    {
        this->uin = r.uin;
        this->name = r.name;
        this->visittime = r.visittime;
        this->portrait = r.portrait;
        this->lastmood = r.lastmood;
        return *this;
    }

    EXPORT_C bool operator==(const Visitor& l, const Visitor& r)
    {
        return l.uin == r.uin && l.name == r.name && l.visittime == r.visittime && l.portrait == r.portrait && l.lastmood == r.lastmood;
    }

    EXPORT_C bool operator!=(const Visitor&l, const Visitor&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String MessageReply::className()
    {
        return taf::String((const signed char *)"cannon.MessageReply");
    }

    EXPORT_C taf::String MessageReply::structName() const
    {
        return taf::String((const signed char *)"cannon.MessageReply");
    }

    EXPORT_C MessageReply::MessageReply()
    : uin(0),nickname(taf::String((const signed char *)"")),content(taf::String((const signed char *)"")),pubdate(0)
    {
    }

    EXPORT_C void MessageReply::reset()
    {
        uin = 0;
        nickname = taf::String((const signed char *)"");
        content = taf::String((const signed char *)"");
        pubdate = 0;
    }

    EXPORT_C void MessageReply::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(uin, 1);
        _os.write(nickname, 2);
        _os.write(content, 3);
        if (pubdate != 0)
        {
            _os.write(pubdate, 4);
        }
    }

    EXPORT_C void MessageReply::readFrom(taf::JceInputStream& _is)
    {
        _is.read(uin, 1, true);
        _is.read(nickname, 2, true);
        _is.read(content, 3, true);
        _is.read(pubdate, 4, false);
    }

    EXPORT_C MessageReply& MessageReply::operator=(const MessageReply& r)
    {
        this->uin = r.uin;
        this->nickname = r.nickname;
        this->content = r.content;
        this->pubdate = r.pubdate;
        return *this;
    }

    EXPORT_C bool operator==(const MessageReply& l, const MessageReply& r)
    {
        return l.uin == r.uin && l.nickname == r.nickname && l.content == r.content && l.pubdate == r.pubdate;
    }

    EXPORT_C bool operator!=(const MessageReply&l, const MessageReply&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String GuestMessage::className()
    {
        return taf::String((const signed char *)"cannon.GuestMessage");
    }

    EXPORT_C taf::String GuestMessage::structName() const
    {
        return taf::String((const signed char *)"cannon.GuestMessage");
    }

    EXPORT_C GuestMessage::GuestMessage()
    : msgid(0),userid(0),username(taf::String((const signed char *)"")),content(taf::String((const signed char *)"")),pubdate(0),sign(taf::String((const signed char *)"")),archive(0),replistsize(0),isCommon(true)
    {
    }

    EXPORT_C void GuestMessage::reset()
    {
        msgid = 0;
        userid = 0;
        username = taf::String((const signed char *)"");
        content = taf::String((const signed char *)"");
        pubdate = 0;
        sign = taf::String((const signed char *)"");
        archive = 0;
        replylist.clear();
        replistsize = 0;
        isCommon = true;
    }

    EXPORT_C void GuestMessage::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(msgid, 1);
        _os.write(userid, 2);
        _os.write(username, 3);
        _os.write(content, 4);
        _os.write(pubdate, 5);
        if (!(sign == taf::String((const signed char *)"")))
        {
            _os.write(sign, 6);
        }
        if (archive != 0)
        {
            _os.write(archive, 7);
        }
        if (replylist.size() > 0)
        {
            _os.write(replylist, 8);
        }
        if (replistsize != 0)
        {
            _os.write(replistsize, 9);
        }
        if (isCommon != true)
        {
            _os.write(isCommon, 10);
        }
    }

    EXPORT_C void GuestMessage::readFrom(taf::JceInputStream& _is)
    {
        _is.read(msgid, 1, true);
        _is.read(userid, 2, true);
        _is.read(username, 3, true);
        _is.read(content, 4, true);
        _is.read(pubdate, 5, true);
        _is.read(sign, 6, false);
        _is.read(archive, 7, false);
        _is.read(replylist, 8, false);
        _is.read(replistsize, 9, false);
        _is.read(isCommon, 10, false);
    }

    EXPORT_C GuestMessage& GuestMessage::operator=(const GuestMessage& r)
    {
        this->msgid = r.msgid;
        this->userid = r.userid;
        this->username = r.username;
        this->content = r.content;
        this->pubdate = r.pubdate;
        this->sign = r.sign;
        this->archive = r.archive;
        this->replylist = r.replylist;
        this->replistsize = r.replistsize;
        this->isCommon = r.isCommon;
        return *this;
    }

    EXPORT_C bool operator==(const GuestMessage& l, const GuestMessage& r)
    {
        return l.msgid == r.msgid && l.userid == r.userid && l.username == r.username && l.content == r.content && l.pubdate == r.pubdate && l.sign == r.sign && l.archive == r.archive && l.replylist == r.replylist && l.replistsize == r.replistsize && l.isCommon == r.isCommon;
    }

    EXPORT_C bool operator!=(const GuestMessage&l, const GuestMessage&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String Mood::className()
    {
        return taf::String((const signed char *)"cannon.Mood");
    }

    EXPORT_C taf::String Mood::structName() const
    {
        return taf::String((const signed char *)"cannon.Mood");
    }

    EXPORT_C Mood::Mood()
    : uin(0),nickname(taf::String((const signed char *)"")),moodid(taf::String((const signed char *)"")),emotion(0),content(taf::String((const signed char *)"")),pubdate(0),replycount(0),tid(taf::String((const signed char *)"")),replistsize(0),isforward(true),oricontent(taf::String((const signed char *)"")),orisourcename(taf::String((const signed char *)"")),oriuinname(taf::String((const signed char *)"")),oriuin(0),oricreatetime(0),orisum(0),oritid(taf::String((const signed char *)"")),richtype(0),richval1(taf::String((const signed char *)"")),richval2(taf::String((const signed char *)"")),orit1source(0),t1source(0),addr(taf::String((const signed char *)"")),posx(0),posy(0)
    {
    }

    EXPORT_C void Mood::reset()
    {
        uin = 0;
        nickname = taf::String((const signed char *)"");
        moodid = taf::String((const signed char *)"");
        emotion = 0;
        content = taf::String((const signed char *)"");
        pubdate = 0;
        replycount = 0;
        tid = taf::String((const signed char *)"");
        replylist.clear();
        replistsize = 0;
        isforward = true;
        oricontent = taf::String((const signed char *)"");
        orisourcename = taf::String((const signed char *)"");
        oriuinname = taf::String((const signed char *)"");
        oriuin = 0;
        oricreatetime = 0;
        orisum = 0;
        oritid = taf::String((const signed char *)"");
        richtype = 0;
        richval1 = taf::String((const signed char *)"");
        richval2 = taf::String((const signed char *)"");
        orit1source = 0;
        t1source = 0;
        addr = taf::String((const signed char *)"");
        posx = 0;
        posy = 0;
    }

    EXPORT_C void Mood::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(uin, 1);
        _os.write(nickname, 2);
        _os.write(moodid, 3);
        _os.write(emotion, 4);
        _os.write(content, 5);
        _os.write(pubdate, 6);
        _os.write(replycount, 7);
        if (!(tid == taf::String((const signed char *)"")))
        {
            _os.write(tid, 8);
        }
        if (replylist.size() > 0)
        {
            _os.write(replylist, 9);
        }
        if (replistsize != 0)
        {
            _os.write(replistsize, 10);
        }
        if (isforward != true)
        {
            _os.write(isforward, 11);
        }
        if (!(oricontent == taf::String((const signed char *)"")))
        {
            _os.write(oricontent, 12);
        }
        if (!(orisourcename == taf::String((const signed char *)"")))
        {
            _os.write(orisourcename, 13);
        }
        if (!(oriuinname == taf::String((const signed char *)"")))
        {
            _os.write(oriuinname, 14);
        }
        if (oriuin != 0)
        {
            _os.write(oriuin, 15);
        }
        if (oricreatetime != 0)
        {
            _os.write(oricreatetime, 16);
        }
        if (orisum != 0)
        {
            _os.write(orisum, 17);
        }
        if (!(oritid == taf::String((const signed char *)"")))
        {
            _os.write(oritid, 18);
        }
        if (richtype != 0)
        {
            _os.write(richtype, 19);
        }
        if (!(richval1 == taf::String((const signed char *)"")))
        {
            _os.write(richval1, 20);
        }
        if (!(richval2 == taf::String((const signed char *)"")))
        {
            _os.write(richval2, 21);
        }
        if (orit1source != 0)
        {
            _os.write(orit1source, 22);
        }
        if (t1source != 0)
        {
            _os.write(t1source, 23);
        }
        if (!(addr == taf::String((const signed char *)"")))
        {
            _os.write(addr, 24);
        }
        if (posx != 0)
        {
            _os.write(posx, 25);
        }
        if (posy != 0)
        {
            _os.write(posy, 26);
        }
    }

    EXPORT_C void Mood::readFrom(taf::JceInputStream& _is)
    {
        _is.read(uin, 1, true);
        _is.read(nickname, 2, true);
        _is.read(moodid, 3, true);
        _is.read(emotion, 4, true);
        _is.read(content, 5, true);
        _is.read(pubdate, 6, true);
        _is.read(replycount, 7, true);
        _is.read(tid, 8, false);
        _is.read(replylist, 9, false);
        _is.read(replistsize, 10, false);
        _is.read(isforward, 11, false);
        _is.read(oricontent, 12, false);
        _is.read(orisourcename, 13, false);
        _is.read(oriuinname, 14, false);
        _is.read(oriuin, 15, false);
        _is.read(oricreatetime, 16, false);
        _is.read(orisum, 17, false);
        _is.read(oritid, 18, false);
        _is.read(richtype, 19, false);
        _is.read(richval1, 20, false);
        _is.read(richval2, 21, false);
        _is.read(orit1source, 22, false);
        _is.read(t1source, 23, false);
        _is.read(addr, 24, false);
        _is.read(posx, 25, false);
        _is.read(posy, 26, false);
    }

    EXPORT_C Mood& Mood::operator=(const Mood& r)
    {
        this->uin = r.uin;
        this->nickname = r.nickname;
        this->moodid = r.moodid;
        this->emotion = r.emotion;
        this->content = r.content;
        this->pubdate = r.pubdate;
        this->replycount = r.replycount;
        this->tid = r.tid;
        this->replylist = r.replylist;
        this->replistsize = r.replistsize;
        this->isforward = r.isforward;
        this->oricontent = r.oricontent;
        this->orisourcename = r.orisourcename;
        this->oriuinname = r.oriuinname;
        this->oriuin = r.oriuin;
        this->oricreatetime = r.oricreatetime;
        this->orisum = r.orisum;
        this->oritid = r.oritid;
        this->richtype = r.richtype;
        this->richval1 = r.richval1;
        this->richval2 = r.richval2;
        this->orit1source = r.orit1source;
        this->t1source = r.t1source;
        this->addr = r.addr;
        this->posx = r.posx;
        this->posy = r.posy;
        return *this;
    }

    EXPORT_C bool operator==(const Mood& l, const Mood& r)
    {
        return l.uin == r.uin && l.nickname == r.nickname && l.moodid == r.moodid && l.emotion == r.emotion && l.content == r.content && l.pubdate == r.pubdate && l.replycount == r.replycount && l.tid == r.tid && l.replylist == r.replylist && l.replistsize == r.replistsize && l.isforward == r.isforward && l.oricontent == r.oricontent && l.orisourcename == r.orisourcename && l.oriuinname == r.oriuinname && l.oriuin == r.oriuin && l.oricreatetime == r.oricreatetime && l.orisum == r.orisum && l.oritid == r.oritid && l.richtype == r.richtype && l.richval1 == r.richval1 && l.richval2 == r.richval2 && l.orit1source == r.orit1source && l.t1source == r.t1source && l.addr == r.addr && l.posx == r.posx && l.posy == r.posy;
    }

    EXPORT_C bool operator!=(const Mood&l, const Mood&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String Album::className()
    {
        return taf::String((const signed char *)"cannon.Album");
    }

    EXPORT_C taf::String Album::structName() const
    {
        return taf::String((const signed char *)"cannon.Album");
    }

    EXPORT_C Album::Album()
    : privacy(0),uin(0),albumid(taf::String((const signed char *)"")),title(taf::String((const signed char *)"")),cover(taf::String((const signed char *)"")),total(0),password(taf::String((const signed char *)"")),createtime(0),updatetime(0)
    {
    }

    EXPORT_C void Album::reset()
    {
        privacy = 0;
        uin = 0;
        albumid = taf::String((const signed char *)"");
        title = taf::String((const signed char *)"");
        cover = taf::String((const signed char *)"");
        total = 0;
        password = taf::String((const signed char *)"");
        createtime = 0;
        updatetime = 0;
    }

    EXPORT_C void Album::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(privacy, 1);
        _os.write(uin, 2);
        _os.write(albumid, 3);
        _os.write(title, 4);
        _os.write(cover, 5);
        _os.write(total, 6);
        if (!(password == taf::String((const signed char *)"")))
        {
            _os.write(password, 7);
        }
        if (createtime != 0)
        {
            _os.write(createtime, 8);
        }
        if (updatetime != 0)
        {
            _os.write(updatetime, 9);
        }
    }

    EXPORT_C void Album::readFrom(taf::JceInputStream& _is)
    {
        _is.read(privacy, 1, true);
        _is.read(uin, 2, true);
        _is.read(albumid, 3, true);
        _is.read(title, 4, true);
        _is.read(cover, 5, true);
        _is.read(total, 6, true);
        _is.read(password, 7, false);
        _is.read(createtime, 8, false);
        _is.read(updatetime, 9, false);
    }

    EXPORT_C Album& Album::operator=(const Album& r)
    {
        this->privacy = r.privacy;
        this->uin = r.uin;
        this->albumid = r.albumid;
        this->title = r.title;
        this->cover = r.cover;
        this->total = r.total;
        this->password = r.password;
        this->createtime = r.createtime;
        this->updatetime = r.updatetime;
        return *this;
    }

    EXPORT_C bool operator==(const Album& l, const Album& r)
    {
        return l.privacy == r.privacy && l.uin == r.uin && l.albumid == r.albumid && l.title == r.title && l.cover == r.cover && l.total == r.total && l.password == r.password && l.createtime == r.createtime && l.updatetime == r.updatetime;
    }

    EXPORT_C bool operator!=(const Album&l, const Album&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String Photo::className()
    {
        return taf::String((const signed char *)"cannon.Photo");
    }

    EXPORT_C taf::String Photo::structName() const
    {
        return taf::String((const signed char *)"cannon.Photo");
    }

    EXPORT_C Photo::Photo()
    : uin(0),albumid(taf::String((const signed char *)"")),photoid(taf::String((const signed char *)"")),title(taf::String((const signed char *)"")),desc(taf::String((const signed char *)"")),pubdate(0),thumbnail(taf::String((const signed char *)"")),url(taf::String((const signed char *)"")),damnimgurl(taf::String((const signed char *)"")),SmarlSizeUrl(taf::String((const signed char *)"")),albumname(taf::String((const signed char *)"")),albumnum(0),cmtnum(0),width(0),height(0)
    {
    }

    EXPORT_C void Photo::reset()
    {
        uin = 0;
        albumid = taf::String((const signed char *)"");
        photoid = taf::String((const signed char *)"");
        title = taf::String((const signed char *)"");
        desc = taf::String((const signed char *)"");
        pubdate = 0;
        thumbnail = taf::String((const signed char *)"");
        url = taf::String((const signed char *)"");
        damnimgurl = taf::String((const signed char *)"");
        SmarlSizeUrl = taf::String((const signed char *)"");
        albumname = taf::String((const signed char *)"");
        albumnum = 0;
        cmtnum = 0;
        width = 0;
        height = 0;
    }

    EXPORT_C void Photo::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(uin, 1);
        _os.write(albumid, 2);
        _os.write(photoid, 3);
        _os.write(title, 4);
        _os.write(desc, 5);
        _os.write(pubdate, 6);
        _os.write(thumbnail, 7);
        _os.write(url, 8);
        if (!(damnimgurl == taf::String((const signed char *)"")))
        {
            _os.write(damnimgurl, 9);
        }
        if (!(SmarlSizeUrl == taf::String((const signed char *)"")))
        {
            _os.write(SmarlSizeUrl, 10);
        }
        if (!(albumname == taf::String((const signed char *)"")))
        {
            _os.write(albumname, 11);
        }
        if (albumnum != 0)
        {
            _os.write(albumnum, 12);
        }
        if (cmtnum != 0)
        {
            _os.write(cmtnum, 13);
        }
        if (width != 0)
        {
            _os.write(width, 14);
        }
        if (height != 0)
        {
            _os.write(height, 15);
        }
    }

    EXPORT_C void Photo::readFrom(taf::JceInputStream& _is)
    {
        _is.read(uin, 1, true);
        _is.read(albumid, 2, true);
        _is.read(photoid, 3, true);
        _is.read(title, 4, true);
        _is.read(desc, 5, true);
        _is.read(pubdate, 6, true);
        _is.read(thumbnail, 7, true);
        _is.read(url, 8, true);
        _is.read(damnimgurl, 9, false);
        _is.read(SmarlSizeUrl, 10, false);
        _is.read(albumname, 11, false);
        _is.read(albumnum, 12, false);
        _is.read(cmtnum, 13, false);
        _is.read(width, 14, false);
        _is.read(height, 15, false);
    }

    EXPORT_C Photo& Photo::operator=(const Photo& r)
    {
        this->uin = r.uin;
        this->albumid = r.albumid;
        this->photoid = r.photoid;
        this->title = r.title;
        this->desc = r.desc;
        this->pubdate = r.pubdate;
        this->thumbnail = r.thumbnail;
        this->url = r.url;
        this->damnimgurl = r.damnimgurl;
        this->SmarlSizeUrl = r.SmarlSizeUrl;
        this->albumname = r.albumname;
        this->albumnum = r.albumnum;
        this->cmtnum = r.cmtnum;
        this->width = r.width;
        this->height = r.height;
        return *this;
    }

    EXPORT_C bool operator==(const Photo& l, const Photo& r)
    {
        return l.uin == r.uin && l.albumid == r.albumid && l.photoid == r.photoid && l.title == r.title && l.desc == r.desc && l.pubdate == r.pubdate && l.thumbnail == r.thumbnail && l.url == r.url && l.damnimgurl == r.damnimgurl && l.SmarlSizeUrl == r.SmarlSizeUrl && l.albumname == r.albumname && l.albumnum == r.albumnum && l.cmtnum == r.cmtnum && l.width == r.width && l.height == r.height;
    }

    EXPORT_C bool operator!=(const Photo&l, const Photo&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String SmallPhoto::className()
    {
        return taf::String((const signed char *)"cannon.SmallPhoto");
    }

    EXPORT_C taf::String SmallPhoto::structName() const
    {
        return taf::String((const signed char *)"cannon.SmallPhoto");
    }

    EXPORT_C SmallPhoto::SmallPhoto()
    : photoid(taf::String((const signed char *)"")),hascmt(true),title(taf::String((const signed char *)""))
    {
    }

    EXPORT_C void SmallPhoto::reset()
    {
        photoid = taf::String((const signed char *)"");
        hascmt = true;
        title = taf::String((const signed char *)"");
    }

    EXPORT_C void SmallPhoto::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(photoid, 1);
        _os.write(hascmt, 2);
        if (!(title == taf::String((const signed char *)"")))
        {
            _os.write(title, 3);
        }
    }

    EXPORT_C void SmallPhoto::readFrom(taf::JceInputStream& _is)
    {
        _is.read(photoid, 1, true);
        _is.read(hascmt, 2, true);
        _is.read(title, 3, false);
    }

    EXPORT_C SmallPhoto& SmallPhoto::operator=(const SmallPhoto& r)
    {
        this->photoid = r.photoid;
        this->hascmt = r.hascmt;
        this->title = r.title;
        return *this;
    }

    EXPORT_C bool operator==(const SmallPhoto& l, const SmallPhoto& r)
    {
        return l.photoid == r.photoid && l.hascmt == r.hascmt && l.title == r.title;
    }

    EXPORT_C bool operator!=(const SmallPhoto&l, const SmallPhoto&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String QzoneFeed::className()
    {
        return taf::String((const signed char *)"cannon.QzoneFeed");
    }

    EXPORT_C taf::String QzoneFeed::structName() const
    {
        return taf::String((const signed char *)"cannon.QzoneFeed");
    }

    EXPORT_C QzoneFeed::QzoneFeed()
    : state(0),opuin(0),opname(taf::String((const signed char *)"")),pubdate(0),feedname(taf::String((const signed char *)"")),feedtype(0)
    {
    }

    EXPORT_C void QzoneFeed::reset()
    {
        state = 0;
        opuin = 0;
        opname = taf::String((const signed char *)"");
        pubdate = 0;
        feedname = taf::String((const signed char *)"");
        feeddata.clear();
        feedtype = 0;
    }

    EXPORT_C void QzoneFeed::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(state, 1);
        _os.write(opuin, 2);
        _os.write(opname, 3);
        _os.write(pubdate, 4);
        _os.write(feedname, 5);
        _os.write(feeddata, 6);
        if (feedtype != 0)
        {
            _os.write(feedtype, 7);
        }
    }

    EXPORT_C void QzoneFeed::readFrom(taf::JceInputStream& _is)
    {
        _is.read(state, 1, true);
        _is.read(opuin, 2, true);
        _is.read(opname, 3, true);
        _is.read(pubdate, 4, true);
        _is.read(feedname, 5, true);
        _is.read(feeddata, 6, true);
        _is.read(feedtype, 7, false);
    }

    EXPORT_C QzoneFeed& QzoneFeed::operator=(const QzoneFeed& r)
    {
        this->state = r.state;
        this->opuin = r.opuin;
        this->opname = r.opname;
        this->pubdate = r.pubdate;
        this->feedname = r.feedname;
        this->feeddata = r.feeddata;
        this->feedtype = r.feedtype;
        return *this;
    }

    EXPORT_C bool operator==(const QzoneFeed& l, const QzoneFeed& r)
    {
        return l.state == r.state && l.opuin == r.opuin && l.opname == r.opname && l.pubdate == r.pubdate && l.feedname == r.feedname && l.feeddata == r.feeddata && l.feedtype == r.feedtype;
    }

    EXPORT_C bool operator!=(const QzoneFeed&l, const QzoneFeed&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String BlogFeed::className()
    {
        return taf::String((const signed char *)"cannon.BlogFeed");
    }

    EXPORT_C taf::String BlogFeed::structName() const
    {
        return taf::String((const signed char *)"cannon.BlogFeed");
    }

    EXPORT_C BlogFeed::BlogFeed()
    : type(0),blogid(0),title(taf::String((const signed char *)"")),summary(taf::String((const signed char *)"")),image(taf::String((const signed char *)"")),authorid(0),authorname(taf::String((const signed char *)"")),cmtnum(0),comlistsize(0),image2(taf::String((const signed char *)"")),image3(taf::String((const signed char *)""))
    {
    }

    EXPORT_C void BlogFeed::reset()
    {
        type = 0;
        blogid = 0;
        title = taf::String((const signed char *)"");
        summary = taf::String((const signed char *)"");
        image = taf::String((const signed char *)"");
        authorid = 0;
        authorname = taf::String((const signed char *)"");
        cmtnum = 0;
        commnetlist.clear();
        comlistsize = 0;
        image2 = taf::String((const signed char *)"");
        image3 = taf::String((const signed char *)"");
    }

    EXPORT_C void BlogFeed::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(type, 1);
        _os.write(blogid, 2);
        _os.write(title, 3);
        if (!(summary == taf::String((const signed char *)"")))
        {
            _os.write(summary, 4);
        }
        if (!(image == taf::String((const signed char *)"")))
        {
            _os.write(image, 5);
        }
        if (authorid != 0)
        {
            _os.write(authorid, 6);
        }
        if (!(authorname == taf::String((const signed char *)"")))
        {
            _os.write(authorname, 7);
        }
        _os.write(cmtnum, 8);
        if (commnetlist.size() > 0)
        {
            _os.write(commnetlist, 9);
        }
        if (comlistsize != 0)
        {
            _os.write(comlistsize, 10);
        }
        if (!(image2 == taf::String((const signed char *)"")))
        {
            _os.write(image2, 11);
        }
        if (!(image3 == taf::String((const signed char *)"")))
        {
            _os.write(image3, 12);
        }
    }

    EXPORT_C void BlogFeed::readFrom(taf::JceInputStream& _is)
    {
        _is.read(type, 1, true);
        _is.read(blogid, 2, true);
        _is.read(title, 3, true);
        _is.read(summary, 4, false);
        _is.read(image, 5, false);
        _is.read(authorid, 6, false);
        _is.read(authorname, 7, false);
        _is.read(cmtnum, 8, true);
        _is.read(commnetlist, 9, false);
        _is.read(comlistsize, 10, false);
        _is.read(image2, 11, false);
        _is.read(image3, 12, false);
    }

    EXPORT_C BlogFeed& BlogFeed::operator=(const BlogFeed& r)
    {
        this->type = r.type;
        this->blogid = r.blogid;
        this->title = r.title;
        this->summary = r.summary;
        this->image = r.image;
        this->authorid = r.authorid;
        this->authorname = r.authorname;
        this->cmtnum = r.cmtnum;
        this->commnetlist = r.commnetlist;
        this->comlistsize = r.comlistsize;
        this->image2 = r.image2;
        this->image3 = r.image3;
        return *this;
    }

    EXPORT_C bool operator==(const BlogFeed& l, const BlogFeed& r)
    {
        return l.type == r.type && l.blogid == r.blogid && l.title == r.title && l.summary == r.summary && l.image == r.image && l.authorid == r.authorid && l.authorname == r.authorname && l.cmtnum == r.cmtnum && l.commnetlist == r.commnetlist && l.comlistsize == r.comlistsize && l.image2 == r.image2 && l.image3 == r.image3;
    }

    EXPORT_C bool operator!=(const BlogFeed&l, const BlogFeed&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String CommentFeed::className()
    {
        return taf::String((const signed char *)"cannon.CommentFeed");
    }

    EXPORT_C taf::String CommentFeed::structName() const
    {
        return taf::String((const signed char *)"cannon.CommentFeed");
    }

    EXPORT_C CommentFeed::CommentFeed()
    : type(0),uin(0),blogid(0),cmtid(0),title(taf::String((const signed char *)"")),comment(taf::String((const signed char *)"")),pubdate(0),cmtuin(0),cmtnick(taf::String((const signed char *)"")),replynum(0),replistsize(0),cmtimg(taf::String((const signed char *)""))
    {
    }

    EXPORT_C void CommentFeed::reset()
    {
        type = 0;
        uin = 0;
        blogid = 0;
        cmtid = 0;
        title = taf::String((const signed char *)"");
        comment = taf::String((const signed char *)"");
        pubdate = 0;
        replylist.clear();
        cmtuin = 0;
        cmtnick = taf::String((const signed char *)"");
        replynum = 0;
        replistsize = 0;
        cmtimg = taf::String((const signed char *)"");
    }

    EXPORT_C void CommentFeed::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(type, 1);
        _os.write(uin, 2);
        _os.write(blogid, 3);
        _os.write(cmtid, 4);
        _os.write(title, 5);
        _os.write(comment, 6);
        _os.write(pubdate, 7);
        if (replylist.size() > 0)
        {
            _os.write(replylist, 8);
        }
        if (cmtuin != 0)
        {
            _os.write(cmtuin, 9);
        }
        if (!(cmtnick == taf::String((const signed char *)"")))
        {
            _os.write(cmtnick, 10);
        }
        if (replynum != 0)
        {
            _os.write(replynum, 11);
        }
        if (replistsize != 0)
        {
            _os.write(replistsize, 12);
        }
        if (!(cmtimg == taf::String((const signed char *)"")))
        {
            _os.write(cmtimg, 13);
        }
    }

    EXPORT_C void CommentFeed::readFrom(taf::JceInputStream& _is)
    {
        _is.read(type, 1, true);
        _is.read(uin, 2, true);
        _is.read(blogid, 3, true);
        _is.read(cmtid, 4, true);
        _is.read(title, 5, true);
        _is.read(comment, 6, true);
        _is.read(pubdate, 7, true);
        _is.read(replylist, 8, false);
        _is.read(cmtuin, 9, false);
        _is.read(cmtnick, 10, false);
        _is.read(replynum, 11, false);
        _is.read(replistsize, 12, false);
        _is.read(cmtimg, 13, false);
    }

    EXPORT_C CommentFeed& CommentFeed::operator=(const CommentFeed& r)
    {
        this->type = r.type;
        this->uin = r.uin;
        this->blogid = r.blogid;
        this->cmtid = r.cmtid;
        this->title = r.title;
        this->comment = r.comment;
        this->pubdate = r.pubdate;
        this->replylist = r.replylist;
        this->cmtuin = r.cmtuin;
        this->cmtnick = r.cmtnick;
        this->replynum = r.replynum;
        this->replistsize = r.replistsize;
        this->cmtimg = r.cmtimg;
        return *this;
    }

    EXPORT_C bool operator==(const CommentFeed& l, const CommentFeed& r)
    {
        return l.type == r.type && l.uin == r.uin && l.blogid == r.blogid && l.cmtid == r.cmtid && l.title == r.title && l.comment == r.comment && l.pubdate == r.pubdate && l.replylist == r.replylist && l.cmtuin == r.cmtuin && l.cmtnick == r.cmtnick && l.replynum == r.replynum && l.replistsize == r.replistsize && l.cmtimg == r.cmtimg;
    }

    EXPORT_C bool operator!=(const CommentFeed&l, const CommentFeed&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String MessageFeed::className()
    {
        return taf::String((const signed char *)"cannon.MessageFeed");
    }

    EXPORT_C taf::String MessageFeed::structName() const
    {
        return taf::String((const signed char *)"cannon.MessageFeed");
    }

    EXPORT_C MessageFeed::MessageFeed()
    : type(0),uin(0),msgid(0),message(taf::String((const signed char *)"")),pubdate(0),replistsize(0)
    {
    }

    EXPORT_C void MessageFeed::reset()
    {
        type = 0;
        uin = 0;
        msgid = 0;
        message = taf::String((const signed char *)"");
        pubdate = 0;
        replylist.clear();
        replistsize = 0;
    }

    EXPORT_C void MessageFeed::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(type, 1);
        _os.write(uin, 2);
        _os.write(msgid, 3);
        _os.write(message, 4);
        _os.write(pubdate, 5);
        if (replylist.size() > 0)
        {
            _os.write(replylist, 6);
        }
        if (replistsize != 0)
        {
            _os.write(replistsize, 7);
        }
    }

    EXPORT_C void MessageFeed::readFrom(taf::JceInputStream& _is)
    {
        _is.read(type, 1, true);
        _is.read(uin, 2, true);
        _is.read(msgid, 3, true);
        _is.read(message, 4, true);
        _is.read(pubdate, 5, true);
        _is.read(replylist, 6, false);
        _is.read(replistsize, 7, false);
    }

    EXPORT_C MessageFeed& MessageFeed::operator=(const MessageFeed& r)
    {
        this->type = r.type;
        this->uin = r.uin;
        this->msgid = r.msgid;
        this->message = r.message;
        this->pubdate = r.pubdate;
        this->replylist = r.replylist;
        this->replistsize = r.replistsize;
        return *this;
    }

    EXPORT_C bool operator==(const MessageFeed& l, const MessageFeed& r)
    {
        return l.type == r.type && l.uin == r.uin && l.msgid == r.msgid && l.message == r.message && l.pubdate == r.pubdate && l.replylist == r.replylist && l.replistsize == r.replistsize;
    }

    EXPORT_C bool operator!=(const MessageFeed&l, const MessageFeed&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String MoodFeed::className()
    {
        return taf::String((const signed char *)"cannon.MoodFeed");
    }

    EXPORT_C taf::String MoodFeed::structName() const
    {
        return taf::String((const signed char *)"cannon.MoodFeed");
    }

    EXPORT_C MoodFeed::MoodFeed()
    : type(0),moodid(taf::String((const signed char *)"")),mood(taf::String((const signed char *)"")),cmt(taf::String((const signed char *)"")),replycount(0),mooduin(0),moodnick(taf::String((const signed char *)"")),cmtnick(taf::String((const signed char *)"")),cmtuin(0),cmttime(0),act(0),cmtnum(0),cmtid(taf::String((const signed char *)"")),stringmoodid(taf::String((const signed char *)"")),stringcmtid(taf::String((const signed char *)"")),richtype(0),sourcename(taf::String((const signed char *)"")),url1(taf::String((const signed char *)"")),url2(taf::String((const signed char *)"")),replistsize(0),isforward(true),lastfwdcontent(taf::String((const signed char *)"")),orisourcename(taf::String((const signed char *)"")),oriname(taf::String((const signed char *)"")),oriuin(0),oritime(0),orisum(0),orimoodid(taf::String((const signed char *)"")),orit1source(0),t1source(0),addr(taf::String((const signed char *)"")),posx(0),posy(0)
    {
    }

    EXPORT_C void MoodFeed::reset()
    {
        type = 0;
        moodid = taf::String((const signed char *)"");
        mood = taf::String((const signed char *)"");
        cmt = taf::String((const signed char *)"");
        replycount = 0;
        mooduin = 0;
        moodnick = taf::String((const signed char *)"");
        cmtnick = taf::String((const signed char *)"");
        cmtuin = 0;
        cmttime = 0;
        replylist.clear();
        act = 0;
        cmtnum = 0;
        cmtid = taf::String((const signed char *)"");
        stringmoodid = taf::String((const signed char *)"");
        stringcmtid = taf::String((const signed char *)"");
        richtype = 0;
        sourcename = taf::String((const signed char *)"");
        url1 = taf::String((const signed char *)"");
        url2 = taf::String((const signed char *)"");
        replistsize = 0;
        isforward = true;
        lastfwdcontent = taf::String((const signed char *)"");
        orisourcename = taf::String((const signed char *)"");
        oriname = taf::String((const signed char *)"");
        oriuin = 0;
        oritime = 0;
        orisum = 0;
        orimoodid = taf::String((const signed char *)"");
        orit1source = 0;
        t1source = 0;
        addr = taf::String((const signed char *)"");
        posx = 0;
        posy = 0;
    }

    EXPORT_C void MoodFeed::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(type, 1);
        _os.write(moodid, 2);
        _os.write(mood, 3);
        if (!(cmt == taf::String((const signed char *)"")))
        {
            _os.write(cmt, 4);
        }
        if (replycount != 0)
        {
            _os.write(replycount, 5);
        }
        if (mooduin != 0)
        {
            _os.write(mooduin, 6);
        }
        if (!(moodnick == taf::String((const signed char *)"")))
        {
            _os.write(moodnick, 7);
        }
        if (!(cmtnick == taf::String((const signed char *)"")))
        {
            _os.write(cmtnick, 8);
        }
        if (cmtuin != 0)
        {
            _os.write(cmtuin, 9);
        }
        if (cmttime != 0)
        {
            _os.write(cmttime, 10);
        }
        if (replylist.size() > 0)
        {
            _os.write(replylist, 11);
        }
        if (act != 0)
        {
            _os.write(act, 12);
        }
        if (cmtnum != 0)
        {
            _os.write(cmtnum, 13);
        }
        if (!(cmtid == taf::String((const signed char *)"")))
        {
            _os.write(cmtid, 14);
        }
        if (!(stringmoodid == taf::String((const signed char *)"")))
        {
            _os.write(stringmoodid, 15);
        }
        if (!(stringcmtid == taf::String((const signed char *)"")))
        {
            _os.write(stringcmtid, 16);
        }
        if (richtype != 0)
        {
            _os.write(richtype, 17);
        }
        if (!(sourcename == taf::String((const signed char *)"")))
        {
            _os.write(sourcename, 18);
        }
        if (!(url1 == taf::String((const signed char *)"")))
        {
            _os.write(url1, 19);
        }
        if (!(url2 == taf::String((const signed char *)"")))
        {
            _os.write(url2, 20);
        }
        if (replistsize != 0)
        {
            _os.write(replistsize, 21);
        }
        if (isforward != true)
        {
            _os.write(isforward, 22);
        }
        if (!(lastfwdcontent == taf::String((const signed char *)"")))
        {
            _os.write(lastfwdcontent, 23);
        }
        if (!(orisourcename == taf::String((const signed char *)"")))
        {
            _os.write(orisourcename, 24);
        }
        if (!(oriname == taf::String((const signed char *)"")))
        {
            _os.write(oriname, 25);
        }
        if (oriuin != 0)
        {
            _os.write(oriuin, 26);
        }
        if (oritime != 0)
        {
            _os.write(oritime, 27);
        }
        if (orisum != 0)
        {
            _os.write(orisum, 28);
        }
        if (!(orimoodid == taf::String((const signed char *)"")))
        {
            _os.write(orimoodid, 29);
        }
        if (orit1source != 0)
        {
            _os.write(orit1source, 30);
        }
        if (t1source != 0)
        {
            _os.write(t1source, 31);
        }
        if (!(addr == taf::String((const signed char *)"")))
        {
            _os.write(addr, 32);
        }
        if (posx != 0)
        {
            _os.write(posx, 33);
        }
        if (posy != 0)
        {
            _os.write(posy, 34);
        }
    }

    EXPORT_C void MoodFeed::readFrom(taf::JceInputStream& _is)
    {
        _is.read(type, 1, true);
        _is.read(moodid, 2, true);
        _is.read(mood, 3, true);
        _is.read(cmt, 4, false);
        _is.read(replycount, 5, false);
        _is.read(mooduin, 6, false);
        _is.read(moodnick, 7, false);
        _is.read(cmtnick, 8, false);
        _is.read(cmtuin, 9, false);
        _is.read(cmttime, 10, false);
        _is.read(replylist, 11, false);
        _is.read(act, 12, false);
        _is.read(cmtnum, 13, false);
        _is.read(cmtid, 14, false);
        _is.read(stringmoodid, 15, false);
        _is.read(stringcmtid, 16, false);
        _is.read(richtype, 17, false);
        _is.read(sourcename, 18, false);
        _is.read(url1, 19, false);
        _is.read(url2, 20, false);
        _is.read(replistsize, 21, false);
        _is.read(isforward, 22, false);
        _is.read(lastfwdcontent, 23, false);
        _is.read(orisourcename, 24, false);
        _is.read(oriname, 25, false);
        _is.read(oriuin, 26, false);
        _is.read(oritime, 27, false);
        _is.read(orisum, 28, false);
        _is.read(orimoodid, 29, false);
        _is.read(orit1source, 30, false);
        _is.read(t1source, 31, false);
        _is.read(addr, 32, false);
        _is.read(posx, 33, false);
        _is.read(posy, 34, false);
    }

    EXPORT_C MoodFeed& MoodFeed::operator=(const MoodFeed& r)
    {
        this->type = r.type;
        this->moodid = r.moodid;
        this->mood = r.mood;
        this->cmt = r.cmt;
        this->replycount = r.replycount;
        this->mooduin = r.mooduin;
        this->moodnick = r.moodnick;
        this->cmtnick = r.cmtnick;
        this->cmtuin = r.cmtuin;
        this->cmttime = r.cmttime;
        this->replylist = r.replylist;
        this->act = r.act;
        this->cmtnum = r.cmtnum;
        this->cmtid = r.cmtid;
        this->stringmoodid = r.stringmoodid;
        this->stringcmtid = r.stringcmtid;
        this->richtype = r.richtype;
        this->sourcename = r.sourcename;
        this->url1 = r.url1;
        this->url2 = r.url2;
        this->replistsize = r.replistsize;
        this->isforward = r.isforward;
        this->lastfwdcontent = r.lastfwdcontent;
        this->orisourcename = r.orisourcename;
        this->oriname = r.oriname;
        this->oriuin = r.oriuin;
        this->oritime = r.oritime;
        this->orisum = r.orisum;
        this->orimoodid = r.orimoodid;
        this->orit1source = r.orit1source;
        this->t1source = r.t1source;
        this->addr = r.addr;
        this->posx = r.posx;
        this->posy = r.posy;
        return *this;
    }

    EXPORT_C bool operator==(const MoodFeed& l, const MoodFeed& r)
    {
        return l.type == r.type && l.moodid == r.moodid && l.mood == r.mood && l.cmt == r.cmt && l.replycount == r.replycount && l.mooduin == r.mooduin && l.moodnick == r.moodnick && l.cmtnick == r.cmtnick && l.cmtuin == r.cmtuin && l.cmttime == r.cmttime && l.replylist == r.replylist && l.act == r.act && l.cmtnum == r.cmtnum && l.cmtid == r.cmtid && l.stringmoodid == r.stringmoodid && l.stringcmtid == r.stringcmtid && l.richtype == r.richtype && l.sourcename == r.sourcename && l.url1 == r.url1 && l.url2 == r.url2 && l.replistsize == r.replistsize && l.isforward == r.isforward && l.lastfwdcontent == r.lastfwdcontent && l.orisourcename == r.orisourcename && l.oriname == r.oriname && l.oriuin == r.oriuin && l.oritime == r.oritime && l.orisum == r.orisum && l.orimoodid == r.orimoodid && l.orit1source == r.orit1source && l.t1source == r.t1source && l.addr == r.addr && l.posx == r.posx && l.posy == r.posy;
    }

    EXPORT_C bool operator!=(const MoodFeed&l, const MoodFeed&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String PhotoUploadFeed::className()
    {
        return taf::String((const signed char *)"cannon.PhotoUploadFeed");
    }

    EXPORT_C taf::String PhotoUploadFeed::structName() const
    {
        return taf::String((const signed char *)"cannon.PhotoUploadFeed");
    }

    EXPORT_C PhotoUploadFeed::PhotoUploadFeed()
    : albumid(taf::String((const signed char *)"")),albumname(taf::String((const signed char *)"")),photoid(taf::String((const signed char *)"")),photourl(taf::String((const signed char *)"")),privacy(0),photoid2(taf::String((const signed char *)"")),photourl2(taf::String((const signed char *)"")),photoid3(taf::String((const signed char *)"")),photourl3(taf::String((const signed char *)"")),replistsize(0),replycount(0),type(0)
    {
    }

    EXPORT_C void PhotoUploadFeed::reset()
    {
        albumid = taf::String((const signed char *)"");
        albumname = taf::String((const signed char *)"");
        photoid = taf::String((const signed char *)"");
        photourl = taf::String((const signed char *)"");
        privacy = 0;
        photoid2 = taf::String((const signed char *)"");
        photourl2 = taf::String((const signed char *)"");
        photoid3 = taf::String((const signed char *)"");
        photourl3 = taf::String((const signed char *)"");
        replylist.clear();
        replistsize = 0;
        replycount = 0;
        type = 0;
    }

    EXPORT_C void PhotoUploadFeed::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(albumid, 1);
        _os.write(albumname, 2);
        _os.write(photoid, 3);
        _os.write(photourl, 4);
        _os.write(privacy, 5);
        if (!(photoid2 == taf::String((const signed char *)"")))
        {
            _os.write(photoid2, 6);
        }
        if (!(photourl2 == taf::String((const signed char *)"")))
        {
            _os.write(photourl2, 7);
        }
        if (!(photoid3 == taf::String((const signed char *)"")))
        {
            _os.write(photoid3, 8);
        }
        if (!(photourl3 == taf::String((const signed char *)"")))
        {
            _os.write(photourl3, 9);
        }
        if (replylist.size() > 0)
        {
            _os.write(replylist, 10);
        }
        if (replistsize != 0)
        {
            _os.write(replistsize, 11);
        }
        if (replycount != 0)
        {
            _os.write(replycount, 12);
        }
        if (type != 0)
        {
            _os.write(type, 13);
        }
    }

    EXPORT_C void PhotoUploadFeed::readFrom(taf::JceInputStream& _is)
    {
        _is.read(albumid, 1, true);
        _is.read(albumname, 2, true);
        _is.read(photoid, 3, true);
        _is.read(photourl, 4, true);
        _is.read(privacy, 5, true);
        _is.read(photoid2, 6, false);
        _is.read(photourl2, 7, false);
        _is.read(photoid3, 8, false);
        _is.read(photourl3, 9, false);
        _is.read(replylist, 10, false);
        _is.read(replistsize, 11, false);
        _is.read(replycount, 12, false);
        _is.read(type, 13, false);
    }

    EXPORT_C PhotoUploadFeed& PhotoUploadFeed::operator=(const PhotoUploadFeed& r)
    {
        this->albumid = r.albumid;
        this->albumname = r.albumname;
        this->photoid = r.photoid;
        this->photourl = r.photourl;
        this->privacy = r.privacy;
        this->photoid2 = r.photoid2;
        this->photourl2 = r.photourl2;
        this->photoid3 = r.photoid3;
        this->photourl3 = r.photourl3;
        this->replylist = r.replylist;
        this->replistsize = r.replistsize;
        this->replycount = r.replycount;
        this->type = r.type;
        return *this;
    }

    EXPORT_C bool operator==(const PhotoUploadFeed& l, const PhotoUploadFeed& r)
    {
        return l.albumid == r.albumid && l.albumname == r.albumname && l.photoid == r.photoid && l.photourl == r.photourl && l.privacy == r.privacy && l.photoid2 == r.photoid2 && l.photourl2 == r.photourl2 && l.photoid3 == r.photoid3 && l.photourl3 == r.photourl3 && l.replylist == r.replylist && l.replistsize == r.replistsize && l.replycount == r.replycount && l.type == r.type;
    }

    EXPORT_C bool operator!=(const PhotoUploadFeed&l, const PhotoUploadFeed&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String PhotoCommentFeed::className()
    {
        return taf::String((const signed char *)"cannon.PhotoCommentFeed");
    }

    EXPORT_C taf::String PhotoCommentFeed::structName() const
    {
        return taf::String((const signed char *)"cannon.PhotoCommentFeed");
    }

    EXPORT_C PhotoCommentFeed::PhotoCommentFeed()
    : type(0),albumid(taf::String((const signed char *)"")),photoid(taf::String((const signed char *)"")),title(taf::String((const signed char *)"")),photourl(taf::String((const signed char *)"")),comment(taf::String((const signed char *)"")),pubdate(0),cmtuin(0),cmtid(0),albumuin(0),replistsize(0)
    {
    }

    EXPORT_C void PhotoCommentFeed::reset()
    {
        type = 0;
        albumid = taf::String((const signed char *)"");
        photoid = taf::String((const signed char *)"");
        title = taf::String((const signed char *)"");
        photourl = taf::String((const signed char *)"");
        comment = taf::String((const signed char *)"");
        pubdate = 0;
        replylist.clear();
        cmtuin = 0;
        cmtid = 0;
        albumuin = 0;
        replistsize = 0;
    }

    EXPORT_C void PhotoCommentFeed::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(type, 1);
        _os.write(albumid, 2);
        _os.write(photoid, 3);
        _os.write(title, 4);
        _os.write(photourl, 5);
        _os.write(comment, 6);
        _os.write(pubdate, 7);
        if (replylist.size() > 0)
        {
            _os.write(replylist, 8);
        }
        if (cmtuin != 0)
        {
            _os.write(cmtuin, 9);
        }
        if (cmtid != 0)
        {
            _os.write(cmtid, 10);
        }
        if (albumuin != 0)
        {
            _os.write(albumuin, 11);
        }
        if (replistsize != 0)
        {
            _os.write(replistsize, 12);
        }
    }

    EXPORT_C void PhotoCommentFeed::readFrom(taf::JceInputStream& _is)
    {
        _is.read(type, 1, true);
        _is.read(albumid, 2, true);
        _is.read(photoid, 3, true);
        _is.read(title, 4, true);
        _is.read(photourl, 5, true);
        _is.read(comment, 6, true);
        _is.read(pubdate, 7, true);
        _is.read(replylist, 8, false);
        _is.read(cmtuin, 9, false);
        _is.read(cmtid, 10, false);
        _is.read(albumuin, 11, false);
        _is.read(replistsize, 12, false);
    }

    EXPORT_C PhotoCommentFeed& PhotoCommentFeed::operator=(const PhotoCommentFeed& r)
    {
        this->type = r.type;
        this->albumid = r.albumid;
        this->photoid = r.photoid;
        this->title = r.title;
        this->photourl = r.photourl;
        this->comment = r.comment;
        this->pubdate = r.pubdate;
        this->replylist = r.replylist;
        this->cmtuin = r.cmtuin;
        this->cmtid = r.cmtid;
        this->albumuin = r.albumuin;
        this->replistsize = r.replistsize;
        return *this;
    }

    EXPORT_C bool operator==(const PhotoCommentFeed& l, const PhotoCommentFeed& r)
    {
        return l.type == r.type && l.albumid == r.albumid && l.photoid == r.photoid && l.title == r.title && l.photourl == r.photourl && l.comment == r.comment && l.pubdate == r.pubdate && l.replylist == r.replylist && l.cmtuin == r.cmtuin && l.cmtid == r.cmtid && l.albumuin == r.albumuin && l.replistsize == r.replistsize;
    }

    EXPORT_C bool operator!=(const PhotoCommentFeed&l, const PhotoCommentFeed&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String MoreFeed::className()
    {
        return taf::String((const signed char *)"cannon.MoreFeed");
    }

    EXPORT_C taf::String MoreFeed::structName() const
    {
        return taf::String((const signed char *)"cannon.MoreFeed");
    }

    EXPORT_C MoreFeed::MoreFeed()
    : begintime(0),endtime(0),sum(0)
    {
    }

    EXPORT_C void MoreFeed::reset()
    {
        begintime = 0;
        endtime = 0;
        sum = 0;
    }

    EXPORT_C void MoreFeed::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(begintime, 1);
        _os.write(endtime, 2);
        _os.write(sum, 3);
    }

    EXPORT_C void MoreFeed::readFrom(taf::JceInputStream& _is)
    {
        _is.read(begintime, 1, true);
        _is.read(endtime, 2, true);
        _is.read(sum, 3, true);
    }

    EXPORT_C MoreFeed& MoreFeed::operator=(const MoreFeed& r)
    {
        this->begintime = r.begintime;
        this->endtime = r.endtime;
        this->sum = r.sum;
        return *this;
    }

    EXPORT_C bool operator==(const MoreFeed& l, const MoreFeed& r)
    {
        return l.begintime == r.begintime && l.endtime == r.endtime && l.sum == r.sum;
    }

    EXPORT_C bool operator!=(const MoreFeed&l, const MoreFeed&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String ShareFeed::className()
    {
        return taf::String((const signed char *)"cannon.ShareFeed");
    }

    EXPORT_C taf::String ShareFeed::structName() const
    {
        return taf::String((const signed char *)"cannon.ShareFeed");
    }

    EXPORT_C ShareFeed::ShareFeed()
    : type(0),title(taf::String((const signed char *)"")),reason(taf::String((const signed char *)"")),resuin(0),reanickname(taf::String((const signed char *)"")),resid(taf::String((const signed char *)"")),shareuin(0),sharenickname(taf::String((const signed char *)"")),sharecount(0),albumId(taf::String((const signed char *)"")),imalistsize(0),summary(taf::String((const signed char *)"")),isPassive(true)
    {
    }

    EXPORT_C void ShareFeed::reset()
    {
        type = 0;
        title = taf::String((const signed char *)"");
        reason = taf::String((const signed char *)"");
        resuin = 0;
        reanickname = taf::String((const signed char *)"");
        resid = taf::String((const signed char *)"");
        shareuin = 0;
        sharenickname = taf::String((const signed char *)"");
        sharecount = 0;
        albumId = taf::String((const signed char *)"");
        imageslist.clear();
        imalistsize = 0;
        summary = taf::String((const signed char *)"");
        isPassive = true;
    }

    EXPORT_C void ShareFeed::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(type, 1);
        _os.write(title, 2);
        _os.write(reason, 3);
        _os.write(resuin, 4);
        _os.write(reanickname, 5);
        _os.write(resid, 6);
        _os.write(shareuin, 7);
        _os.write(sharenickname, 8);
        _os.write(sharecount, 9);
        if (!(albumId == taf::String((const signed char *)"")))
        {
            _os.write(albumId, 10);
        }
        if (imageslist.size() > 0)
        {
            _os.write(imageslist, 11);
        }
        if (imalistsize != 0)
        {
            _os.write(imalistsize, 12);
        }
        _os.write(summary, 13);
        _os.write(isPassive, 14);
    }

    EXPORT_C void ShareFeed::readFrom(taf::JceInputStream& _is)
    {
        _is.read(type, 1, true);
        _is.read(title, 2, true);
        _is.read(reason, 3, true);
        _is.read(resuin, 4, true);
        _is.read(reanickname, 5, true);
        _is.read(resid, 6, true);
        _is.read(shareuin, 7, true);
        _is.read(sharenickname, 8, true);
        _is.read(sharecount, 9, true);
        _is.read(albumId, 10, false);
        _is.read(imageslist, 11, false);
        _is.read(imalistsize, 12, false);
        _is.read(summary, 13, true);
        _is.read(isPassive, 14, true);
    }

    EXPORT_C ShareFeed& ShareFeed::operator=(const ShareFeed& r)
    {
        this->type = r.type;
        this->title = r.title;
        this->reason = r.reason;
        this->resuin = r.resuin;
        this->reanickname = r.reanickname;
        this->resid = r.resid;
        this->shareuin = r.shareuin;
        this->sharenickname = r.sharenickname;
        this->sharecount = r.sharecount;
        this->albumId = r.albumId;
        this->imageslist = r.imageslist;
        this->imalistsize = r.imalistsize;
        this->summary = r.summary;
        this->isPassive = r.isPassive;
        return *this;
    }

    EXPORT_C bool operator==(const ShareFeed& l, const ShareFeed& r)
    {
        return l.type == r.type && l.title == r.title && l.reason == r.reason && l.resuin == r.resuin && l.reanickname == r.reanickname && l.resid == r.resid && l.shareuin == r.shareuin && l.sharenickname == r.sharenickname && l.sharecount == r.sharecount && l.albumId == r.albumId && l.imageslist == r.imageslist && l.imalistsize == r.imalistsize && l.summary == r.summary && l.isPassive == r.isPassive;
    }

    EXPORT_C bool operator!=(const ShareFeed&l, const ShareFeed&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String SimpleUserInfo::className()
    {
        return taf::String((const signed char *)"cannon.SimpleUserInfo");
    }

    EXPORT_C taf::String SimpleUserInfo::structName() const
    {
        return taf::String((const signed char *)"cannon.SimpleUserInfo");
    }

    EXPORT_C SimpleUserInfo::SimpleUserInfo()
    : userid(0),nickname(taf::String((const signed char *)"")),qzonename(taf::String((const signed char *)"")),gender(0),viplevel(0),remark(taf::String((const signed char *)""))
    {
    }

    EXPORT_C void SimpleUserInfo::reset()
    {
        userid = 0;
        nickname = taf::String((const signed char *)"");
        qzonename = taf::String((const signed char *)"");
        gender = 0;
        viplevel = 0;
        remark = taf::String((const signed char *)"");
    }

    EXPORT_C void SimpleUserInfo::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(userid, 1);
        _os.write(nickname, 2);
        _os.write(qzonename, 3);
        _os.write(gender, 4);
        _os.write(viplevel, 5);
        _os.write(remark, 6);
    }

    EXPORT_C void SimpleUserInfo::readFrom(taf::JceInputStream& _is)
    {
        _is.read(userid, 1, true);
        _is.read(nickname, 2, true);
        _is.read(qzonename, 3, true);
        _is.read(gender, 4, true);
        _is.read(viplevel, 5, true);
        _is.read(remark, 6, true);
    }

    EXPORT_C SimpleUserInfo& SimpleUserInfo::operator=(const SimpleUserInfo& r)
    {
        this->userid = r.userid;
        this->nickname = r.nickname;
        this->qzonename = r.qzonename;
        this->gender = r.gender;
        this->viplevel = r.viplevel;
        this->remark = r.remark;
        return *this;
    }

    EXPORT_C bool operator==(const SimpleUserInfo& l, const SimpleUserInfo& r)
    {
        return l.userid == r.userid && l.nickname == r.nickname && l.qzonename == r.qzonename && l.gender == r.gender && l.viplevel == r.viplevel && l.remark == r.remark;
    }

    EXPORT_C bool operator!=(const SimpleUserInfo&l, const SimpleUserInfo&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String PhotoReply::className()
    {
        return taf::String((const signed char *)"cannon.PhotoReply");
    }

    EXPORT_C taf::String PhotoReply::structName() const
    {
        return taf::String((const signed char *)"cannon.PhotoReply");
    }

    EXPORT_C PhotoReply::PhotoReply()
    : replyuserid(0),replytime(0),replycontent(taf::String((const signed char *)"")),replynickname(taf::String((const signed char *)""))
    {
    }

    EXPORT_C void PhotoReply::reset()
    {
        replyuserid = 0;
        replytime = 0;
        replycontent = taf::String((const signed char *)"");
        replynickname = taf::String((const signed char *)"");
    }

    EXPORT_C void PhotoReply::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(replyuserid, 1);
        _os.write(replytime, 2);
        _os.write(replycontent, 3);
        _os.write(replynickname, 4);
    }

    EXPORT_C void PhotoReply::readFrom(taf::JceInputStream& _is)
    {
        _is.read(replyuserid, 1, true);
        _is.read(replytime, 2, true);
        _is.read(replycontent, 3, true);
        _is.read(replynickname, 4, true);
    }

    EXPORT_C PhotoReply& PhotoReply::operator=(const PhotoReply& r)
    {
        this->replyuserid = r.replyuserid;
        this->replytime = r.replytime;
        this->replycontent = r.replycontent;
        this->replynickname = r.replynickname;
        return *this;
    }

    EXPORT_C bool operator==(const PhotoReply& l, const PhotoReply& r)
    {
        return l.replyuserid == r.replyuserid && l.replytime == r.replytime && l.replycontent == r.replycontent && l.replynickname == r.replynickname;
    }

    EXPORT_C bool operator!=(const PhotoReply&l, const PhotoReply&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String PhotoCmt::className()
    {
        return taf::String((const signed char *)"cannon.PhotoCmt");
    }

    EXPORT_C taf::String PhotoCmt::structName() const
    {
        return taf::String((const signed char *)"cannon.PhotoCmt");
    }

    EXPORT_C PhotoCmt::PhotoCmt()
    : cmtuserid(0),albumid(taf::String((const signed char *)"")),lloc(taf::String((const signed char *)"")),sloc(taf::String((const signed char *)"")),cmtid(0),cmttime(0),cmtcontent(taf::String((const signed char *)"")),nickname(taf::String((const signed char *)"")),replynum(0)
    {
    }

    EXPORT_C void PhotoCmt::reset()
    {
        cmtuserid = 0;
        albumid = taf::String((const signed char *)"");
        lloc = taf::String((const signed char *)"");
        sloc = taf::String((const signed char *)"");
        cmtid = 0;
        cmttime = 0;
        cmtcontent = taf::String((const signed char *)"");
        nickname = taf::String((const signed char *)"");
        replynum = 0;
        replylist.clear();
    }

    EXPORT_C void PhotoCmt::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(cmtuserid, 1);
        _os.write(albumid, 2);
        _os.write(lloc, 3);
        _os.write(sloc, 4);
        _os.write(cmtid, 5);
        _os.write(cmttime, 6);
        _os.write(cmtcontent, 7);
        _os.write(nickname, 8);
        _os.write(replynum, 9);
        if (replylist.size() > 0)
        {
            _os.write(replylist, 10);
        }
    }

    EXPORT_C void PhotoCmt::readFrom(taf::JceInputStream& _is)
    {
        _is.read(cmtuserid, 1, true);
        _is.read(albumid, 2, true);
        _is.read(lloc, 3, true);
        _is.read(sloc, 4, true);
        _is.read(cmtid, 5, true);
        _is.read(cmttime, 6, true);
        _is.read(cmtcontent, 7, true);
        _is.read(nickname, 8, true);
        _is.read(replynum, 9, true);
        _is.read(replylist, 10, false);
    }

    EXPORT_C PhotoCmt& PhotoCmt::operator=(const PhotoCmt& r)
    {
        this->cmtuserid = r.cmtuserid;
        this->albumid = r.albumid;
        this->lloc = r.lloc;
        this->sloc = r.sloc;
        this->cmtid = r.cmtid;
        this->cmttime = r.cmttime;
        this->cmtcontent = r.cmtcontent;
        this->nickname = r.nickname;
        this->replynum = r.replynum;
        this->replylist = r.replylist;
        return *this;
    }

    EXPORT_C bool operator==(const PhotoCmt& l, const PhotoCmt& r)
    {
        return l.cmtuserid == r.cmtuserid && l.albumid == r.albumid && l.lloc == r.lloc && l.sloc == r.sloc && l.cmtid == r.cmtid && l.cmttime == r.cmttime && l.cmtcontent == r.cmtcontent && l.nickname == r.nickname && l.replynum == r.replynum && l.replylist == r.replylist;
    }

    EXPORT_C bool operator!=(const PhotoCmt&l, const PhotoCmt&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String Category::className()
    {
        return taf::String((const signed char *)"cannon.Category");
    }

    EXPORT_C taf::String Category::structName() const
    {
        return taf::String((const signed char *)"cannon.Category");
    }

    EXPORT_C Category::Category()
    : name(taf::String((const signed char *)"")),count(0)
    {
    }

    EXPORT_C void Category::reset()
    {
        name = taf::String((const signed char *)"");
        count = 0;
    }

    EXPORT_C void Category::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(name, 1);
        _os.write(count, 2);
    }

    EXPORT_C void Category::readFrom(taf::JceInputStream& _is)
    {
        _is.read(name, 1, true);
        _is.read(count, 2, true);
    }

    EXPORT_C Category& Category::operator=(const Category& r)
    {
        this->name = r.name;
        this->count = r.count;
        return *this;
    }

    EXPORT_C bool operator==(const Category& l, const Category& r)
    {
        return l.name == r.name && l.count == r.count;
    }

    EXPORT_C bool operator!=(const Category&l, const Category&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String Group::className()
    {
        return taf::String((const signed char *)"cannon.Group");
    }

    EXPORT_C taf::String Group::structName() const
    {
        return taf::String((const signed char *)"cannon.Group");
    }

    EXPORT_C Group::Group()
    : groupid(0),groupname(taf::String((const signed char *)""))
    {
    }

    EXPORT_C void Group::reset()
    {
        groupid = 0;
        groupname = taf::String((const signed char *)"");
    }

    EXPORT_C void Group::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(groupid, 1);
        _os.write(groupname, 2);
    }

    EXPORT_C void Group::readFrom(taf::JceInputStream& _is)
    {
        _is.read(groupid, 1, true);
        _is.read(groupname, 2, true);
    }

    EXPORT_C Group& Group::operator=(const Group& r)
    {
        this->groupid = r.groupid;
        this->groupname = r.groupname;
        return *this;
    }

    EXPORT_C bool operator==(const Group& l, const Group& r)
    {
        return l.groupid == r.groupid && l.groupname == r.groupname;
    }

    EXPORT_C bool operator!=(const Group&l, const Group&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String GroupFriend::className()
    {
        return taf::String((const signed char *)"cannon.GroupFriend");
    }

    EXPORT_C taf::String GroupFriend::structName() const
    {
        return taf::String((const signed char *)"cannon.GroupFriend");
    }

    EXPORT_C GroupFriend::GroupFriend()
    : uin(0),groupid(0)
    {
    }

    EXPORT_C void GroupFriend::reset()
    {
        uin = 0;
        groupid = 0;
    }

    EXPORT_C void GroupFriend::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(uin, 1);
        _os.write(groupid, 2);
    }

    EXPORT_C void GroupFriend::readFrom(taf::JceInputStream& _is)
    {
        _is.read(uin, 1, true);
        _is.read(groupid, 2, true);
    }

    EXPORT_C GroupFriend& GroupFriend::operator=(const GroupFriend& r)
    {
        this->uin = r.uin;
        this->groupid = r.groupid;
        return *this;
    }

    EXPORT_C bool operator==(const GroupFriend& l, const GroupFriend& r)
    {
        return l.uin == r.uin && l.groupid == r.groupid;
    }

    EXPORT_C bool operator!=(const GroupFriend&l, const GroupFriend&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String FriendNick::className()
    {
        return taf::String((const signed char *)"cannon.FriendNick");
    }

    EXPORT_C taf::String FriendNick::structName() const
    {
        return taf::String((const signed char *)"cannon.FriendNick");
    }

    EXPORT_C FriendNick::FriendNick()
    : uin(0),nick(taf::String((const signed char *)"")),remark(taf::String((const signed char *)""))
    {
    }

    EXPORT_C void FriendNick::reset()
    {
        uin = 0;
        nick = taf::String((const signed char *)"");
        remark = taf::String((const signed char *)"");
    }

    EXPORT_C void FriendNick::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(uin, 1);
        _os.write(nick, 2);
        _os.write(remark, 3);
    }

    EXPORT_C void FriendNick::readFrom(taf::JceInputStream& _is)
    {
        _is.read(uin, 1, true);
        _is.read(nick, 2, true);
        _is.read(remark, 3, true);
    }

    EXPORT_C FriendNick& FriendNick::operator=(const FriendNick& r)
    {
        this->uin = r.uin;
        this->nick = r.nick;
        this->remark = r.remark;
        return *this;
    }

    EXPORT_C bool operator==(const FriendNick& l, const FriendNick& r)
    {
        return l.uin == r.uin && l.nick == r.nick && l.remark == r.remark;
    }

    EXPORT_C bool operator!=(const FriendNick&l, const FriendNick&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String GPSPoint::className()
    {
        return taf::String((const signed char *)"cannon.GPSPoint");
    }

    EXPORT_C taf::String GPSPoint::structName() const
    {
        return taf::String((const signed char *)"cannon.GPSPoint");
    }

    EXPORT_C GPSPoint::GPSPoint()
    : lat(900000000),lon(900000000),alt(-10000000)
    {
    }

    EXPORT_C void GPSPoint::reset()
    {
        lat = 900000000;
        lon = 900000000;
        alt = -10000000;
    }

    EXPORT_C void GPSPoint::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(lat, 0);
        _os.write(lon, 1);
        _os.write(alt, 2);
    }

    EXPORT_C void GPSPoint::readFrom(taf::JceInputStream& _is)
    {
        _is.read(lat, 0, true);
        _is.read(lon, 1, true);
        _is.read(alt, 2, true);
    }

    EXPORT_C GPSPoint& GPSPoint::operator=(const GPSPoint& r)
    {
        this->lat = r.lat;
        this->lon = r.lon;
        this->alt = r.alt;
        return *this;
    }

    EXPORT_C bool operator==(const GPSPoint& l, const GPSPoint& r)
    {
        return l.lat == r.lat && l.lon == r.lon && l.alt == r.alt;
    }

    EXPORT_C bool operator!=(const GPSPoint&l, const GPSPoint&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String GSMCell::className()
    {
        return taf::String((const signed char *)"cannon.GSMCell");
    }

    EXPORT_C taf::String GSMCell::structName() const
    {
        return taf::String((const signed char *)"cannon.GSMCell");
    }

    EXPORT_C GSMCell::GSMCell()
    : mcc(-1),mnc(-1),lac(-1),cellid(-1)
    {
    }

    EXPORT_C void GSMCell::reset()
    {
        mcc = -1;
        mnc = -1;
        lac = -1;
        cellid = -1;
    }

    EXPORT_C void GSMCell::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(mcc, 0);
        _os.write(mnc, 1);
        _os.write(lac, 2);
        _os.write(cellid, 3);
    }

    EXPORT_C void GSMCell::readFrom(taf::JceInputStream& _is)
    {
        _is.read(mcc, 0, true);
        _is.read(mnc, 1, true);
        _is.read(lac, 2, true);
        _is.read(cellid, 3, true);
    }

    EXPORT_C GSMCell& GSMCell::operator=(const GSMCell& r)
    {
        this->mcc = r.mcc;
        this->mnc = r.mnc;
        this->lac = r.lac;
        this->cellid = r.cellid;
        return *this;
    }

    EXPORT_C bool operator==(const GSMCell& l, const GSMCell& r)
    {
        return l.mcc == r.mcc && l.mnc == r.mnc && l.lac == r.lac && l.cellid == r.cellid;
    }

    EXPORT_C bool operator!=(const GSMCell&l, const GSMCell&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String BirthdayInfo::className()
    {
        return taf::String((const signed char *)"cannon.BirthdayInfo");
    }

    EXPORT_C taf::String BirthdayInfo::structName() const
    {
        return taf::String((const signed char *)"cannon.BirthdayInfo");
    }

    EXPORT_C BirthdayInfo::BirthdayInfo()
    : uin(0),nick(taf::String((const signed char *)"")),isSend(true),birthday(taf::String((const signed char *)""))
    {
    }

    EXPORT_C void BirthdayInfo::reset()
    {
        uin = 0;
        nick = taf::String((const signed char *)"");
        isSend = true;
        birthday = taf::String((const signed char *)"");
    }

    EXPORT_C void BirthdayInfo::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(uin, 1);
        _os.write(nick, 2);
        _os.write(isSend, 3);
        _os.write(birthday, 4);
    }

    EXPORT_C void BirthdayInfo::readFrom(taf::JceInputStream& _is)
    {
        _is.read(uin, 1, true);
        _is.read(nick, 2, true);
        _is.read(isSend, 3, true);
        _is.read(birthday, 4, true);
    }

    EXPORT_C BirthdayInfo& BirthdayInfo::operator=(const BirthdayInfo& r)
    {
        this->uin = r.uin;
        this->nick = r.nick;
        this->isSend = r.isSend;
        this->birthday = r.birthday;
        return *this;
    }

    EXPORT_C bool operator==(const BirthdayInfo& l, const BirthdayInfo& r)
    {
        return l.uin == r.uin && l.nick == r.nick && l.isSend == r.isSend && l.birthday == r.birthday;
    }

    EXPORT_C bool operator!=(const BirthdayInfo&l, const BirthdayInfo&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String GiftFeed::className()
    {
        return taf::String((const signed char *)"cannon.GiftFeed");
    }

    EXPORT_C taf::String GiftFeed::structName() const
    {
        return taf::String((const signed char *)"cannon.GiftFeed");
    }

    EXPORT_C GiftFeed::GiftFeed()
    : content(taf::String((const signed char *)"")),itemId(0),itemName(taf::String((const signed char *)"")),preFormat(taf::String((const signed char *)""))
    {
    }

    EXPORT_C void GiftFeed::reset()
    {
        content = taf::String((const signed char *)"");
        itemId = 0;
        itemName = taf::String((const signed char *)"");
        preFormat = taf::String((const signed char *)"");
    }

    EXPORT_C void GiftFeed::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(content, 1);
        _os.write(itemId, 2);
        _os.write(itemName, 3);
        _os.write(preFormat, 4);
    }

    EXPORT_C void GiftFeed::readFrom(taf::JceInputStream& _is)
    {
        _is.read(content, 1, true);
        _is.read(itemId, 2, true);
        _is.read(itemName, 3, true);
        _is.read(preFormat, 4, true);
    }

    EXPORT_C GiftFeed& GiftFeed::operator=(const GiftFeed& r)
    {
        this->content = r.content;
        this->itemId = r.itemId;
        this->itemName = r.itemName;
        this->preFormat = r.preFormat;
        return *this;
    }

    EXPORT_C bool operator==(const GiftFeed& l, const GiftFeed& r)
    {
        return l.content == r.content && l.itemId == r.itemId && l.itemName == r.itemName && l.preFormat == r.preFormat;
    }

    EXPORT_C bool operator!=(const GiftFeed&l, const GiftFeed&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String PrivBlog::className()
    {
        return taf::String((const signed char *)"cannon.PrivBlog");
    }

    EXPORT_C taf::String PrivBlog::structName() const
    {
        return taf::String((const signed char *)"cannon.PrivBlog");
    }

    EXPORT_C PrivBlog::PrivBlog()
    : title(taf::String((const signed char *)"")),blogId(0),pubTime(0),isQuote(true),isHasPic(true),content(taf::String((const signed char *)""))
    {
    }

    EXPORT_C void PrivBlog::reset()
    {
        title = taf::String((const signed char *)"");
        blogId = 0;
        pubTime = 0;
        isQuote = true;
        isHasPic = true;
        content = taf::String((const signed char *)"");
    }

    EXPORT_C void PrivBlog::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(title, 1);
        _os.write(blogId, 2);
        _os.write(pubTime, 3);
        _os.write(isQuote, 4);
        _os.write(isHasPic, 5);
        if (!(content == taf::String((const signed char *)"")))
        {
            _os.write(content, 6);
        }
    }

    EXPORT_C void PrivBlog::readFrom(taf::JceInputStream& _is)
    {
        _is.read(title, 1, true);
        _is.read(blogId, 2, true);
        _is.read(pubTime, 3, true);
        _is.read(isQuote, 4, true);
        _is.read(isHasPic, 5, true);
        _is.read(content, 6, false);
    }

    EXPORT_C PrivBlog& PrivBlog::operator=(const PrivBlog& r)
    {
        this->title = r.title;
        this->blogId = r.blogId;
        this->pubTime = r.pubTime;
        this->isQuote = r.isQuote;
        this->isHasPic = r.isHasPic;
        this->content = r.content;
        return *this;
    }

    EXPORT_C bool operator==(const PrivBlog& l, const PrivBlog& r)
    {
        return l.title == r.title && l.blogId == r.blogId && l.pubTime == r.pubTime && l.isQuote == r.isQuote && l.isHasPic == r.isHasPic && l.content == r.content;
    }

    EXPORT_C bool operator!=(const PrivBlog&l, const PrivBlog&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String UserSimpleStatus::className()
    {
        return taf::String((const signed char *)"cannon.UserSimpleStatus");
    }

    EXPORT_C taf::String UserSimpleStatus::structName() const
    {
        return taf::String((const signed char *)"cannon.UserSimpleStatus");
    }

    EXPORT_C UserSimpleStatus::UserSimpleStatus()
    : mood(taf::String((const signed char *)"")),photourl(taf::String((const signed char *)"")),nickname(taf::String((const signed char *)"")),uin(0)
    {
    }

    EXPORT_C void UserSimpleStatus::reset()
    {
        mood = taf::String((const signed char *)"");
        photourl = taf::String((const signed char *)"");
        nickname = taf::String((const signed char *)"");
        uin = 0;
    }

    EXPORT_C void UserSimpleStatus::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(mood, 1);
        _os.write(photourl, 2);
        _os.write(nickname, 3);
        _os.write(uin, 4);
    }

    EXPORT_C void UserSimpleStatus::readFrom(taf::JceInputStream& _is)
    {
        _is.read(mood, 1, true);
        _is.read(photourl, 2, true);
        _is.read(nickname, 3, true);
        _is.read(uin, 4, true);
    }

    EXPORT_C UserSimpleStatus& UserSimpleStatus::operator=(const UserSimpleStatus& r)
    {
        this->mood = r.mood;
        this->photourl = r.photourl;
        this->nickname = r.nickname;
        this->uin = r.uin;
        return *this;
    }

    EXPORT_C bool operator==(const UserSimpleStatus& l, const UserSimpleStatus& r)
    {
        return l.mood == r.mood && l.photourl == r.photourl && l.nickname == r.nickname && l.uin == r.uin;
    }

    EXPORT_C bool operator!=(const UserSimpleStatus&l, const UserSimpleStatus&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String ClientQzoneApp::className()
    {
        return taf::String((const signed char *)"cannon.ClientQzoneApp");
    }

    EXPORT_C taf::String ClientQzoneApp::structName() const
    {
        return taf::String((const signed char *)"cannon.ClientQzoneApp");
    }

    EXPORT_C ClientQzoneApp::ClientQzoneApp()
    : title(taf::String((const signed char *)"")),mark(0),url(taf::String((const signed char *)"")),pic(taf::String((const signed char *)"")),appid(0)
    {
    }

    EXPORT_C void ClientQzoneApp::reset()
    {
        title = taf::String((const signed char *)"");
        mark = 0;
        url = taf::String((const signed char *)"");
        pic = taf::String((const signed char *)"");
        appid = 0;
    }

    EXPORT_C void ClientQzoneApp::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(title, 1);
        _os.write(mark, 2);
        _os.write(url, 3);
        _os.write(pic, 4);
        if (appid != 0)
        {
            _os.write(appid, 5);
        }
    }

    EXPORT_C void ClientQzoneApp::readFrom(taf::JceInputStream& _is)
    {
        _is.read(title, 1, true);
        _is.read(mark, 2, true);
        _is.read(url, 3, true);
        _is.read(pic, 4, true);
        _is.read(appid, 5, false);
    }

    EXPORT_C ClientQzoneApp& ClientQzoneApp::operator=(const ClientQzoneApp& r)
    {
        this->title = r.title;
        this->mark = r.mark;
        this->url = r.url;
        this->pic = r.pic;
        this->appid = r.appid;
        return *this;
    }

    EXPORT_C bool operator==(const ClientQzoneApp& l, const ClientQzoneApp& r)
    {
        return l.title == r.title && l.mark == r.mark && l.url == r.url && l.pic == r.pic && l.appid == r.appid;
    }

    EXPORT_C bool operator!=(const ClientQzoneApp&l, const ClientQzoneApp&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String QzoneAndMsfMsgPkg::className()
    {
        return taf::String((const signed char *)"cannon.QzoneAndMsfMsgPkg");
    }

    EXPORT_C taf::String QzoneAndMsfMsgPkg::structName() const
    {
        return taf::String((const signed char *)"cannon.QzoneAndMsfMsgPkg");
    }

    EXPORT_C QzoneAndMsfMsgPkg::QzoneAndMsfMsgPkg()
    : version(0),commandID(0),loginUIN(0),loginNickname(taf::String((const signed char *)"")),isNightModeSkin(0),apnID(0),sid(taf::String((const signed char *)""))
    {
    }

    EXPORT_C void QzoneAndMsfMsgPkg::reset()
    {
        version = 0;
        commandID = 0;
        loginUIN = 0;
        loginNickname = taf::String((const signed char *)"");
        isNightModeSkin = 0;
        apnID = 0;
        sid = taf::String((const signed char *)"");
        paramBuf.clear();
    }

    EXPORT_C void QzoneAndMsfMsgPkg::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(version, 1);
        _os.write(commandID, 2);
        _os.write(loginUIN, 3);
        _os.write(loginNickname, 4);
        _os.write(isNightModeSkin, 5);
        _os.write(apnID, 6);
        _os.write(sid, 7);
        if (paramBuf.size() > 0)
        {
            _os.write(paramBuf, 8);
        }
    }

    EXPORT_C void QzoneAndMsfMsgPkg::readFrom(taf::JceInputStream& _is)
    {
        _is.read(version, 1, true);
        _is.read(commandID, 2, true);
        _is.read(loginUIN, 3, true);
        _is.read(loginNickname, 4, true);
        _is.read(isNightModeSkin, 5, true);
        _is.read(apnID, 6, true);
        _is.read(sid, 7, true);
        _is.read(paramBuf, 8, false);
    }

    EXPORT_C QzoneAndMsfMsgPkg& QzoneAndMsfMsgPkg::operator=(const QzoneAndMsfMsgPkg& r)
    {
        this->version = r.version;
        this->commandID = r.commandID;
        this->loginUIN = r.loginUIN;
        this->loginNickname = r.loginNickname;
        this->isNightModeSkin = r.isNightModeSkin;
        this->apnID = r.apnID;
        this->sid = r.sid;
        this->paramBuf = r.paramBuf;
        return *this;
    }

    EXPORT_C bool operator==(const QzoneAndMsfMsgPkg& l, const QzoneAndMsfMsgPkg& r)
    {
        return l.version == r.version && l.commandID == r.commandID && l.loginUIN == r.loginUIN && l.loginNickname == r.loginNickname && l.isNightModeSkin == r.isNightModeSkin && l.apnID == r.apnID && l.sid == r.sid && l.paramBuf == r.paramBuf;
    }

    EXPORT_C bool operator!=(const QzoneAndMsfMsgPkg&l, const QzoneAndMsfMsgPkg&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String OpenMainpageMsgParam::className()
    {
        return taf::String((const signed char *)"cannon.OpenMainpageMsgParam");
    }

    EXPORT_C taf::String OpenMainpageMsgParam::structName() const
    {
        return taf::String((const signed char *)"cannon.OpenMainpageMsgParam");
    }

    EXPORT_C OpenMainpageMsgParam::OpenMainpageMsgParam()
    : tabID(0),friendUIN(0)
    {
    }

    EXPORT_C void OpenMainpageMsgParam::reset()
    {
        tabID = 0;
        friendUIN = 0;
    }

    EXPORT_C void OpenMainpageMsgParam::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(tabID, 1);
        _os.write(friendUIN, 2);
    }

    EXPORT_C void OpenMainpageMsgParam::readFrom(taf::JceInputStream& _is)
    {
        _is.read(tabID, 1, true);
        _is.read(friendUIN, 2, true);
    }

    EXPORT_C OpenMainpageMsgParam& OpenMainpageMsgParam::operator=(const OpenMainpageMsgParam& r)
    {
        this->tabID = r.tabID;
        this->friendUIN = r.friendUIN;
        return *this;
    }

    EXPORT_C bool operator==(const OpenMainpageMsgParam& l, const OpenMainpageMsgParam& r)
    {
        return l.tabID == r.tabID && l.friendUIN == r.friendUIN;
    }

    EXPORT_C bool operator!=(const OpenMainpageMsgParam&l, const OpenMainpageMsgParam&r)
    {
        return !(l == r);
    }

    EXPORT_C taf::String OpenFeedMsgParam::className()
    {
        return taf::String((const signed char *)"cannon.OpenFeedMsgParam");
    }

    EXPORT_C taf::String OpenFeedMsgParam::structName() const
    {
        return taf::String((const signed char *)"cannon.OpenFeedMsgParam");
    }

    EXPORT_C OpenFeedMsgParam::OpenFeedMsgParam()
    : feedIndex(0)
    {
    }

    EXPORT_C void OpenFeedMsgParam::reset()
    {
        feedData.clear();
        feedIndex = 0;
    }

    EXPORT_C void OpenFeedMsgParam::writeTo(taf::JceOutputStream& _os) const
    {
        _os.write(feedData, 1);
        if (feedIndex != 0)
        {
            _os.write(feedIndex, 2);
        }
    }

    EXPORT_C void OpenFeedMsgParam::readFrom(taf::JceInputStream& _is)
    {
        _is.read(feedData, 1, true);
        _is.read(feedIndex, 2, false);
    }

    EXPORT_C OpenFeedMsgParam& OpenFeedMsgParam::operator=(const OpenFeedMsgParam& r)
    {
        this->feedData = r.feedData;
        this->feedIndex = r.feedIndex;
        return *this;
    }

    EXPORT_C bool operator==(const OpenFeedMsgParam& l, const OpenFeedMsgParam& r)
    {
        return l.feedData == r.feedData && l.feedIndex == r.feedIndex;
    }

    EXPORT_C bool operator!=(const OpenFeedMsgParam&l, const OpenFeedMsgParam&r)
    {
        return !(l == r);
    }


}



