#include "Patch.h"

using namespace taf;

class PatchImp : public Patch
{
public:
    /**
     *
     */
    PatchImp();

    /**
     * 初始化
     *
     * @return int
     */
    virtual void initialize();

    /**
     * 退出
     */
    virtual void destroy() {};

    /**
    * 获取路径下所有文件列表信息
    * @param path, 目录路径, 相对_directory的路径, 不能有..
    * @param vector<FileInfo>, 文件列表信息
    * @return int
    */
    int listFileInfo(const string &path, vector<FileInfo> &vf, JceCurrentPtr current);

    /**
    * 下载文件
    * @param file, 文件完全路径
    * @param pos, 从什么位置开始下载
    * @return vector<byte>, 文件内容
    */
    int download(const string &file, int pos, vector<char> &vb, JceCurrentPtr current);

    /**
    * 获取路径下所有文件列表信息
    * @param path, 目录路径, 相对_directory的路径, 不能有..
    * @param vector<FileInfo>, 文件列表信息
    * @return int
    */
    int listFileInfoSubborn(const string &path, vector<FileInfo> &vf, JceCurrentPtr current);

    /**
    * 下载文件
    * @param file, 文件的绝对路径，不再重组文件路径
    * @param pos, 从什么位置开始下载
    * @param vector<byte>, 文件内容
    * @param int, 0:读取表示成功, 1:读取到文件末尾了, <0: 读取失败
    */
    int downloadSubborn(const string & file, int pos, vector<char> &vb, JceCurrentPtr current);

protected:
	int __listFileInfo(const string &path, vector<FileInfo> &vf);

	int __downloadFromMem (const string & file, size_t pos, vector<char> & vb);

    int __downloadFromFile(const string & file, size_t pos, vector<char> & vb);

protected:
    /**
     * 目录
     */
    string _directory;

    /**
     * 每次同步大小
     */
    size_t _iSize;
};


