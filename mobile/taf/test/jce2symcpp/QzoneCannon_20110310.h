#ifndef __QZONECANNON_20110310_H_
#define __QZONECANNON_20110310_H_

#include "Jce_sym.h"
#include "sys/types.h"
#include "netinet/in.h"


namespace cannon
{
    struct Profile : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C Profile();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Int64 uin;
        taf::String nickname;
        taf::String qzonename;
        taf::Char vip;
        taf::Char gender;
        taf::Char age;
        taf::Int32 birthday;
        taf::Char astro;
        taf::Char marriage;
        taf::String country;
        taf::String province;
        taf::String city;
        taf::String Portrait;
        taf::String birthdaystr;

        IMPORT_C Profile& operator=(const Profile& r);
    };
    IMPORT_C bool operator==(const Profile& l, const Profile& r);
    IMPORT_C bool operator!=(const Profile&l, const Profile&r);

    struct BlogTitle : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C BlogTitle();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Int64 uin;
        taf::Int32 blogid;
        taf::String title;
        taf::Int32 read;
        taf::Int32 comment;
        taf::Int32 effect;
        taf::Int32 pubdate;
        taf::String category;
        taf::String content;
        taf::Int32 rights;

        IMPORT_C BlogTitle& operator=(const BlogTitle& r);
    };
    IMPORT_C bool operator==(const BlogTitle& l, const BlogTitle& r);
    IMPORT_C bool operator!=(const BlogTitle&l, const BlogTitle&r);

    struct BlogInfo : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C BlogInfo();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Int64 uin;
        taf::Int32 blogid;
        taf::String title;
        taf::String content;
        taf::String sign;
        taf::String cat;
        taf::Int32 pubdate;
        taf::Int32 effect;
        taf::Int32 read;
        taf::Int32 comment;
        taf::Int64 authorid;
        taf::String authorname;

        IMPORT_C BlogInfo& operator=(const BlogInfo& r);
    };
    IMPORT_C bool operator==(const BlogInfo& l, const BlogInfo& r);
    IMPORT_C bool operator!=(const BlogInfo&l, const BlogInfo&r);

    struct CommentReply : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C CommentReply();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Int64 uin;
        taf::String nickname;
        taf::String content;
        taf::Int32 pubdate;

        IMPORT_C CommentReply& operator=(const CommentReply& r);
    };
    IMPORT_C bool operator==(const CommentReply& l, const CommentReply& r);
    IMPORT_C bool operator!=(const CommentReply&l, const CommentReply&r);

    struct BlogComment : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C BlogComment();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Int32 blogid;
        taf::Int32 cmtid;
        taf::Int64 userid;
        taf::String username;
        taf::String content;
        taf::Int32 pubdate;
        taf::String sign;
        taf::Int32 archive;
        taf::Int32 effect;
        taf::JArray<cannon::CommentReply> replylist;
        taf::Int32 replyCount;
        taf::Int32 replistsize;

        IMPORT_C BlogComment& operator=(const BlogComment& r);
    };
    IMPORT_C bool operator==(const BlogComment& l, const BlogComment& r);
    IMPORT_C bool operator!=(const BlogComment&l, const BlogComment&r);

    struct Visitor : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C Visitor();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Int64 uin;
        taf::String name;
        taf::Int32 visittime;
        taf::String portrait;
        taf::String lastmood;

        IMPORT_C Visitor& operator=(const Visitor& r);
    };
    IMPORT_C bool operator==(const Visitor& l, const Visitor& r);
    IMPORT_C bool operator!=(const Visitor&l, const Visitor&r);

    struct MessageReply : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C MessageReply();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Int64 uin;
        taf::String nickname;
        taf::String content;
        taf::Int32 pubdate;

        IMPORT_C MessageReply& operator=(const MessageReply& r);
    };
    IMPORT_C bool operator==(const MessageReply& l, const MessageReply& r);
    IMPORT_C bool operator!=(const MessageReply&l, const MessageReply&r);

    struct GuestMessage : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C GuestMessage();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Int32 msgid;
        taf::Int64 userid;
        taf::String username;
        taf::String content;
        taf::Int32 pubdate;
        taf::String sign;
        taf::Int32 archive;
        taf::JArray<cannon::MessageReply> replylist;
        taf::Int32 replistsize;
        taf::Bool isCommon;

        IMPORT_C GuestMessage& operator=(const GuestMessage& r);
    };
    IMPORT_C bool operator==(const GuestMessage& l, const GuestMessage& r);
    IMPORT_C bool operator!=(const GuestMessage&l, const GuestMessage&r);

    struct Mood : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C Mood();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Int64 uin;
        taf::String nickname;
        taf::String moodid;
        taf::Int32 emotion;
        taf::String content;
        taf::Int32 pubdate;
        taf::Int32 replycount;
        taf::String tid;
        taf::JArray<cannon::CommentReply> replylist;
        taf::Int32 replistsize;
        taf::Bool isforward;
        taf::String oricontent;
        taf::String orisourcename;
        taf::String oriuinname;
        taf::Int64 oriuin;
        taf::Int32 oricreatetime;
        taf::Int32 orisum;
        taf::String oritid;
        taf::Int32 richtype;
        taf::String richval1;
        taf::String richval2;
        taf::Int32 orit1source;
        taf::Int32 t1source;
        taf::String addr;
        taf::Int32 posx;
        taf::Int32 posy;

        IMPORT_C Mood& operator=(const Mood& r);
    };
    IMPORT_C bool operator==(const Mood& l, const Mood& r);
    IMPORT_C bool operator!=(const Mood&l, const Mood&r);

    struct Album : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C Album();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Char privacy;
        taf::Int64 uin;
        taf::String albumid;
        taf::String title;
        taf::String cover;
        taf::Int32 total;
        taf::String password;
        taf::Int32 createtime;
        taf::Int32 updatetime;

        IMPORT_C Album& operator=(const Album& r);
    };
    IMPORT_C bool operator==(const Album& l, const Album& r);
    IMPORT_C bool operator!=(const Album&l, const Album&r);

    struct Photo : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C Photo();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Int64 uin;
        taf::String albumid;
        taf::String photoid;
        taf::String title;
        taf::String desc;
        taf::Int32 pubdate;
        taf::String thumbnail;
        taf::String url;
        taf::String damnimgurl;
        taf::String SmarlSizeUrl;
        taf::String albumname;
        taf::Int32 albumnum;
        taf::Int32 cmtnum;
        taf::Int32 width;
        taf::Int32 height;

        IMPORT_C Photo& operator=(const Photo& r);
    };
    IMPORT_C bool operator==(const Photo& l, const Photo& r);
    IMPORT_C bool operator!=(const Photo&l, const Photo&r);

    struct SmallPhoto : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C SmallPhoto();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::String photoid;
        taf::Bool hascmt;
        taf::String title;

        IMPORT_C SmallPhoto& operator=(const SmallPhoto& r);
    };
    IMPORT_C bool operator==(const SmallPhoto& l, const SmallPhoto& r);
    IMPORT_C bool operator!=(const SmallPhoto&l, const SmallPhoto&r);

    struct QzoneFeed : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C QzoneFeed();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Char state;
        taf::Int64 opuin;
        taf::String opname;
        taf::Int32 pubdate;
        taf::String feedname;
        taf::JArray<taf::Char> feeddata;
        taf::Char feedtype;

        IMPORT_C QzoneFeed& operator=(const QzoneFeed& r);
    };
    IMPORT_C bool operator==(const QzoneFeed& l, const QzoneFeed& r);
    IMPORT_C bool operator!=(const QzoneFeed&l, const QzoneFeed&r);

    struct BlogFeed : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C BlogFeed();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Char type;
        taf::Int32 blogid;
        taf::String title;
        taf::String summary;
        taf::String image;
        taf::Int64 authorid;
        taf::String authorname;
        taf::Int32 cmtnum;
        taf::JArray<cannon::BlogComment> commnetlist;
        taf::Int32 comlistsize;
        taf::String image2;
        taf::String image3;

        IMPORT_C BlogFeed& operator=(const BlogFeed& r);
    };
    IMPORT_C bool operator==(const BlogFeed& l, const BlogFeed& r);
    IMPORT_C bool operator!=(const BlogFeed&l, const BlogFeed&r);

    struct CommentFeed : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C CommentFeed();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Char type;
        taf::Int64 uin;
        taf::Int32 blogid;
        taf::Int32 cmtid;
        taf::String title;
        taf::String comment;
        taf::Int32 pubdate;
        taf::JArray<cannon::CommentReply> replylist;
        taf::Int64 cmtuin;
        taf::String cmtnick;
        taf::Int32 replynum;
        taf::Int32 replistsize;
        taf::String cmtimg;

        IMPORT_C CommentFeed& operator=(const CommentFeed& r);
    };
    IMPORT_C bool operator==(const CommentFeed& l, const CommentFeed& r);
    IMPORT_C bool operator!=(const CommentFeed&l, const CommentFeed&r);

    struct MessageFeed : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C MessageFeed();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Char type;
        taf::Int64 uin;
        taf::Int32 msgid;
        taf::String message;
        taf::Int32 pubdate;
        taf::JArray<cannon::MessageReply> replylist;
        taf::Int32 replistsize;

        IMPORT_C MessageFeed& operator=(const MessageFeed& r);
    };
    IMPORT_C bool operator==(const MessageFeed& l, const MessageFeed& r);
    IMPORT_C bool operator!=(const MessageFeed&l, const MessageFeed&r);

    struct MoodFeed : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C MoodFeed();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Char type;
        taf::String moodid;
        taf::String mood;
        taf::String cmt;
        taf::Int32 replycount;
        taf::Int64 mooduin;
        taf::String moodnick;
        taf::String cmtnick;
        taf::Int64 cmtuin;
        taf::Int32 cmttime;
        taf::JArray<cannon::Mood> replylist;
        taf::Int32 act;
        taf::Int32 cmtnum;
        taf::String cmtid;
        taf::String stringmoodid;
        taf::String stringcmtid;
        taf::Int32 richtype;
        taf::String sourcename;
        taf::String url1;
        taf::String url2;
        taf::Int32 replistsize;
        taf::Bool isforward;
        taf::String lastfwdcontent;
        taf::String orisourcename;
        taf::String oriname;
        taf::Int64 oriuin;
        taf::Int32 oritime;
        taf::Int32 orisum;
        taf::String orimoodid;
        taf::Int32 orit1source;
        taf::Int32 t1source;
        taf::String addr;
        taf::Int32 posx;
        taf::Int32 posy;

        IMPORT_C MoodFeed& operator=(const MoodFeed& r);
    };
    IMPORT_C bool operator==(const MoodFeed& l, const MoodFeed& r);
    IMPORT_C bool operator!=(const MoodFeed&l, const MoodFeed&r);

    struct PhotoUploadFeed : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C PhotoUploadFeed();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::String albumid;
        taf::String albumname;
        taf::String photoid;
        taf::String photourl;
        taf::Char privacy;
        taf::String photoid2;
        taf::String photourl2;
        taf::String photoid3;
        taf::String photourl3;
        taf::JArray<cannon::CommentReply> replylist;
        taf::Int32 replistsize;
        taf::Int32 replycount;
        taf::Char type;

        IMPORT_C PhotoUploadFeed& operator=(const PhotoUploadFeed& r);
    };
    IMPORT_C bool operator==(const PhotoUploadFeed& l, const PhotoUploadFeed& r);
    IMPORT_C bool operator!=(const PhotoUploadFeed&l, const PhotoUploadFeed&r);

    struct PhotoCommentFeed : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C PhotoCommentFeed();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Char type;
        taf::String albumid;
        taf::String photoid;
        taf::String title;
        taf::String photourl;
        taf::String comment;
        taf::Int32 pubdate;
        taf::JArray<cannon::CommentReply> replylist;
        taf::Int64 cmtuin;
        taf::Int32 cmtid;
        taf::Int64 albumuin;
        taf::Int32 replistsize;

        IMPORT_C PhotoCommentFeed& operator=(const PhotoCommentFeed& r);
    };
    IMPORT_C bool operator==(const PhotoCommentFeed& l, const PhotoCommentFeed& r);
    IMPORT_C bool operator!=(const PhotoCommentFeed&l, const PhotoCommentFeed&r);

    struct MoreFeed : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C MoreFeed();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Int32 begintime;
        taf::Int32 endtime;
        taf::Int32 sum;

        IMPORT_C MoreFeed& operator=(const MoreFeed& r);
    };
    IMPORT_C bool operator==(const MoreFeed& l, const MoreFeed& r);
    IMPORT_C bool operator!=(const MoreFeed&l, const MoreFeed&r);

    struct ShareFeed : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C ShareFeed();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Int32 type;
        taf::String title;
        taf::String reason;
        taf::Int64 resuin;
        taf::String reanickname;
        taf::String resid;
        taf::Int64 shareuin;
        taf::String sharenickname;
        taf::Int32 sharecount;
        taf::String albumId;
        taf::JArray<taf::String> imageslist;
        taf::Int32 imalistsize;
        taf::String summary;
        taf::Bool isPassive;

        IMPORT_C ShareFeed& operator=(const ShareFeed& r);
    };
    IMPORT_C bool operator==(const ShareFeed& l, const ShareFeed& r);
    IMPORT_C bool operator!=(const ShareFeed&l, const ShareFeed&r);

    struct SimpleUserInfo : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C SimpleUserInfo();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Int64 userid;
        taf::String nickname;
        taf::String qzonename;
        taf::Int32 gender;
        taf::Int32 viplevel;
        taf::String remark;

        IMPORT_C SimpleUserInfo& operator=(const SimpleUserInfo& r);
    };
    IMPORT_C bool operator==(const SimpleUserInfo& l, const SimpleUserInfo& r);
    IMPORT_C bool operator!=(const SimpleUserInfo&l, const SimpleUserInfo&r);

    struct PhotoReply : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C PhotoReply();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Int64 replyuserid;
        taf::Int32 replytime;
        taf::String replycontent;
        taf::String replynickname;

        IMPORT_C PhotoReply& operator=(const PhotoReply& r);
    };
    IMPORT_C bool operator==(const PhotoReply& l, const PhotoReply& r);
    IMPORT_C bool operator!=(const PhotoReply&l, const PhotoReply&r);

    struct PhotoCmt : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C PhotoCmt();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Int64 cmtuserid;
        taf::String albumid;
        taf::String lloc;
        taf::String sloc;
        taf::Int32 cmtid;
        taf::Int32 cmttime;
        taf::String cmtcontent;
        taf::String nickname;
        taf::Int32 replynum;
        taf::JArray<cannon::PhotoReply> replylist;

        IMPORT_C PhotoCmt& operator=(const PhotoCmt& r);
    };
    IMPORT_C bool operator==(const PhotoCmt& l, const PhotoCmt& r);
    IMPORT_C bool operator!=(const PhotoCmt&l, const PhotoCmt&r);

    struct Category : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C Category();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::String name;
        taf::Int32 count;

        IMPORT_C Category& operator=(const Category& r);
    };
    IMPORT_C bool operator==(const Category& l, const Category& r);
    IMPORT_C bool operator!=(const Category&l, const Category&r);

    struct Group : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C Group();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Int32 groupid;
        taf::String groupname;

        IMPORT_C Group& operator=(const Group& r);
    };
    IMPORT_C bool operator==(const Group& l, const Group& r);
    IMPORT_C bool operator!=(const Group&l, const Group&r);

    struct GroupFriend : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C GroupFriend();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Int64 uin;
        taf::Int32 groupid;

        IMPORT_C GroupFriend& operator=(const GroupFriend& r);
    };
    IMPORT_C bool operator==(const GroupFriend& l, const GroupFriend& r);
    IMPORT_C bool operator!=(const GroupFriend&l, const GroupFriend&r);

    struct FriendNick : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C FriendNick();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Int64 uin;
        taf::String nick;
        taf::String remark;

        IMPORT_C FriendNick& operator=(const FriendNick& r);
    };
    IMPORT_C bool operator==(const FriendNick& l, const FriendNick& r);
    IMPORT_C bool operator!=(const FriendNick&l, const FriendNick&r);

    struct GPSPoint : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C GPSPoint();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Int32 lat;
        taf::Int32 lon;
        taf::Int32 alt;

        IMPORT_C GPSPoint& operator=(const GPSPoint& r);
    };
    IMPORT_C bool operator==(const GPSPoint& l, const GPSPoint& r);
    IMPORT_C bool operator!=(const GPSPoint&l, const GPSPoint&r);

    struct GSMCell : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C GSMCell();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Short mcc;
        taf::Short mnc;
        taf::Int32 lac;
        taf::Int32 cellid;

        IMPORT_C GSMCell& operator=(const GSMCell& r);
    };
    IMPORT_C bool operator==(const GSMCell& l, const GSMCell& r);
    IMPORT_C bool operator!=(const GSMCell&l, const GSMCell&r);

    struct BirthdayInfo : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C BirthdayInfo();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Int64 uin;
        taf::String nick;
        taf::Bool isSend;
        taf::String birthday;

        IMPORT_C BirthdayInfo& operator=(const BirthdayInfo& r);
    };
    IMPORT_C bool operator==(const BirthdayInfo& l, const BirthdayInfo& r);
    IMPORT_C bool operator!=(const BirthdayInfo&l, const BirthdayInfo&r);

    struct GiftFeed : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C GiftFeed();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::String content;
        taf::Int64 itemId;
        taf::String itemName;
        taf::String preFormat;

        IMPORT_C GiftFeed& operator=(const GiftFeed& r);
    };
    IMPORT_C bool operator==(const GiftFeed& l, const GiftFeed& r);
    IMPORT_C bool operator!=(const GiftFeed&l, const GiftFeed&r);

    struct PrivBlog : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C PrivBlog();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::String title;
        taf::Int32 blogId;
        taf::Int32 pubTime;
        taf::Bool isQuote;
        taf::Bool isHasPic;
        taf::String content;

        IMPORT_C PrivBlog& operator=(const PrivBlog& r);
    };
    IMPORT_C bool operator==(const PrivBlog& l, const PrivBlog& r);
    IMPORT_C bool operator!=(const PrivBlog&l, const PrivBlog&r);

    struct UserSimpleStatus : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C UserSimpleStatus();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::String mood;
        taf::String photourl;
        taf::String nickname;
        taf::Int64 uin;

        IMPORT_C UserSimpleStatus& operator=(const UserSimpleStatus& r);
    };
    IMPORT_C bool operator==(const UserSimpleStatus& l, const UserSimpleStatus& r);
    IMPORT_C bool operator!=(const UserSimpleStatus&l, const UserSimpleStatus&r);

    struct ClientQzoneApp : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C ClientQzoneApp();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::String title;
        taf::Int32 mark;
        taf::String url;
        taf::String pic;
        taf::Int32 appid;

        IMPORT_C ClientQzoneApp& operator=(const ClientQzoneApp& r);
    };
    IMPORT_C bool operator==(const ClientQzoneApp& l, const ClientQzoneApp& r);
    IMPORT_C bool operator!=(const ClientQzoneApp&l, const ClientQzoneApp&r);

    struct QzoneAndMsfMsgPkg : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C QzoneAndMsfMsgPkg();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Int32 version;
        taf::Int32 commandID;
        taf::Int64 loginUIN;
        taf::String loginNickname;
        taf::Char isNightModeSkin;
        taf::Char apnID;
        taf::String sid;
        taf::JArray<taf::Char> paramBuf;

        IMPORT_C QzoneAndMsfMsgPkg& operator=(const QzoneAndMsfMsgPkg& r);
    };
    IMPORT_C bool operator==(const QzoneAndMsfMsgPkg& l, const QzoneAndMsfMsgPkg& r);
    IMPORT_C bool operator!=(const QzoneAndMsfMsgPkg&l, const QzoneAndMsfMsgPkg&r);

    struct OpenMainpageMsgParam : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C OpenMainpageMsgParam();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::Char tabID;
        taf::Int64 friendUIN;

        IMPORT_C OpenMainpageMsgParam& operator=(const OpenMainpageMsgParam& r);
    };
    IMPORT_C bool operator==(const OpenMainpageMsgParam& l, const OpenMainpageMsgParam& r);
    IMPORT_C bool operator!=(const OpenMainpageMsgParam&l, const OpenMainpageMsgParam&r);

    struct OpenFeedMsgParam : public taf::JceStructBase
    {
    public:
        IMPORT_C static taf::String className();
        IMPORT_C taf::String structName() const;
        IMPORT_C OpenFeedMsgParam();
        IMPORT_C void reset();
        IMPORT_C void writeTo(taf::JceOutputStream& _os) const;
        IMPORT_C void readFrom(taf::JceInputStream& _is);
    public:
        taf::JArray<taf::Char> feedData;
        taf::Int32 feedIndex;

        IMPORT_C OpenFeedMsgParam& operator=(const OpenFeedMsgParam& r);
    };
    IMPORT_C bool operator==(const OpenFeedMsgParam& l, const OpenFeedMsgParam& r);
    IMPORT_C bool operator!=(const OpenFeedMsgParam&l, const OpenFeedMsgParam&r);


}



#endif
