xmlproxy的自动测试工具ats
ats使用172.27.205.110的测试环境
testcase是Test.TestServer的测试用例，example下是几个请求的xml的格式
使用方法：
1、将要输入的xml文件保存成request.*.xml的形式，放在ats同一目录下，请求的结果会与response.*.xml进行比较，如果response.*.xml不存在或者不一样，就会测试失败。另外测试的结果会保存到strResponse文件。
