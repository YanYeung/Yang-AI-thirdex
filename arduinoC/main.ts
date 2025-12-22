//% color="#993300" iconWidth=50 iconHeight=40
namespace XunfeiAI {
    //% block="设置科大讯飞文字识别 APPID[APPID] APIKey[APIKey] " blockType="command"
    //% APPID.shadow="string" APPID.defl="your_appid"
    //% APIKey.shadow="string" APIKey.defl="your_apikey"
    export function setOCRCredentials(parameter: any, block: any) {
        let appid = parameter.APPID.code;
        let apikey = parameter.APIKey.code;
        Generator.addInclude('xf_ocr_include', '#include <XunfeiOCRAPI.h>');
        Generator.addObject('xf_ocr_obj', 'XunfeiOCRAPI', 'xunfeiOCR;');
        Generator.addCode(`xunfeiOCR.init(${appid}, ${apikey});`);
    }

    //% block="设置科大讯飞人脸识别 APPID[APPID] APIKey[APIKey] APISecret[APISecret]" blockType="command"
    //% APPID.shadow="string" APPID.defl="your_appid"
    //% APIKey.shadow="string" APIKey.defl="your_apikey"
    //% APISecret.shadow="string" APISecret.defl="your_apisecret"
    export function setFaceCredentials(parameter: any, block: any) {
        let appid = parameter.APPID.code;
        let apikey = parameter.APIKey.code;
        let apisecret = parameter.APISecret.code;
        Generator.addInclude('xf_face_include', '#include <XunfeiFaceAPI.h>');
        Generator.addObject('xf_face_obj', 'XunfeiFaceAPI','xunfeiFace;');
        Generator.addCode(`xunfeiFace.init(${appid}, ${apikey}, ${apisecret});`);
    }

    //% block="科大讯飞【OCR识别】 图片Base64[IMAGE] 返回文本" blockType="reporter"
    //% IMAGE.shadow="string" IMAGE.defl="4AAQSkZJRgABAQ"
    export function ocrRecognize(parameter: any, block: any) {
        let image = parameter.IMAGE.code;
        Generator.addInclude('xf_ocr_include', '#include <XunfeiOCRAPI.h>');
        Generator.addObject('xf_ocr_obj', 'XunfeiOCRAPI','xunfeiOCR;');
        Generator.addCode([`xunfeiOCR.recognize(${image}).text`, Generator.ORDER_UNARY_POSTFIX]);
    }

    //% block="科大讯飞【表情识别】 图片Base64[IMAGE]" blockType="reporter"
    //% IMAGE.shadow="string" IMAGE.defl="4AAQSkZJRgABAQ"
    export function faceDetectExpression(parameter: any, block: any) {
        let image = parameter.IMAGE.code;
        Generator.addInclude('xf_face_include', '#include <XunfeiFaceAPI.h>');
        Generator.addObject('xf_face_obj', 'XunfeiFaceAPI','xunfeiFace;');
        Generator.addCode([`xunfeiFace.detect(${image}).expression`, Generator.ORDER_UNARY_POSTFIX]);
    }

    //% block="将BMP图片[PATH]转换为JPG Base64" blockType="reporter"
    //% PATH.shadow="string" PATH.defl="/photo.bmp"
    export function convertBmpToJpgBase64(parameter: any, block: any) {
        let path = parameter.PATH.code;
        Generator.addInclude('k10_img_proc_include', '#include "K10ImageProcessor.h"');
        Generator.addObject('k10_img_proc_obj', 'K10ImageProcessor', 'processor;');
        Generator.addSetup('k10_img_proc_begin', 'processor.begin();');
        Generator.addCode([`processor.processImageToBase64(${path})`, Generator.ORDER_UNARY_POSTFIX]);
    }

    //% block="中文[STR]的字符数" blockType="reporter"
    //% STR.shadow="string" STR.defl="你好"
    export function countUtf8Characters(parameter: any, block: any) {
        let str = parameter.STR.code;
        Generator.addInclude('count_utf8_include', '#include "countUtf8Characters.h"');
        Generator.addCode([`countUtf8Characters(${str})`, Generator.ORDER_UNARY_POSTFIX]);
    }

    //% block="中文[STR]的第[INDEX]个字符" blockType="reporter"
    //% STR.shadow="string" STR.defl="你好"
    //% INDEX.shadow="number" INDEX.defl=1
    export function getUtf8Char(parameter: any, block: any) {
        let str = parameter.STR.code;
        let index = parameter.INDEX.code;
        Generator.addInclude('count_utf8_include', '#include "countUtf8Characters.h"');
        Generator.addCode([`getUtf8CharPointer(${str}, ${index})`, Generator.ORDER_UNARY_POSTFIX]);
    }

    //% block="中文[STR]的[CHAR]的索引" blockType="reporter"
    //% STR.shadow="string" STR.defl="你好"
    //% CHAR.shadow="string" CHAR.defl="好"
    export function getUtf8CharIndex(parameter: any, block: any) {
        let str = parameter.STR.code;
        let char = parameter.CHAR.code;
        Generator.addInclude('count_utf8_include', '#include "countUtf8Characters.h"');
        Generator.addCode([`getUtf8CharIndex(${str}, ${char})`, Generator.ORDER_UNARY_POSTFIX]);
    }
    
    
    //% block="初始化本地分类器 地址[URL]" blockType="command"
    //% URL.shadow="string" URL.defl="http://192.168.1.100:8000"
    export function initClassifier(parameter: any, block: any) {
        let url = parameter.URL.code;
        Generator.addInclude('local_classifier_include', '#include <LocalClassifier.h>');
        Generator.addObject('local_classifier_obj', 'LocalClassifier', 'classifier;');
        Generator.addSetup('local_classifier_init', `classifier.setBaseUrl(${url});`);
    }

    //% block="文本分类[TEXT]结果" blockType="reporter"
    //% TEXT.shadow="string" TEXT.defl="hello"
    export function classifyText(parameter: any, block: any) {
        let text = parameter.TEXT.code;
        Generator.addInclude('local_classifier_include', '#include <LocalClassifier.h>');
        Generator.addObject('local_classifier_obj', 'LocalClassifier', 'classifier;');
        Generator.addCode([`classifier.classifyText(${text})`, Generator.ORDER_UNARY_POSTFIX]);
    }
    
    //% block="图像分类的结果,图片质量[QUALITY]" blockType="reporter"
    //% QUALITY.shadow="number" QUALITY.defl=50
    export function classifyImageRaw(parameter: any, block: any) {
        Generator.addInclude('local_classifier_include', '#include <LocalClassifier.h>');
        Generator.addObject('local_classifier_obj', 'LocalClassifier', 'classifier;');
        let quality = parameter.QUALITY.code;
        Generator.addCode([`classifier.classifyImageRaw(${quality})`, Generator.ORDER_UNARY_POSTFIX]);
    }
    //% block="百度OCR,图片质量[QUALITY]" blockType="reporter"
    //% QUALITY.shadow="number" QUALITY.defl=50
    export function baiduOcrRaw(parameter: any, block: any) {
        Generator.addInclude('local_classifier_include', '#include <LocalClassifier.h>');
        Generator.addObject('local_classifier_obj', 'LocalClassifier', 'classifier;');
        let quality = parameter.QUALITY.code;
        Generator.addCode([`classifier.baiduOcrRaw(${quality})`, Generator.ORDER_UNARY_POSTFIX]);
    }

    //% block="解析JSON[JSON] 获取键[KEY]的值" blockType="reporter"
    //% JSON.shadow="string" JSON.defl="{}"
    //% KEY.shadow="string" KEY.defl="label"
    export function getJsonValue(parameter: any, block: any) {
        let json = parameter.JSON.code;
        let key = parameter.KEY.code;
        Generator.addInclude('local_classifier_include', '#include <LocalClassifier.h>');
        Generator.addObject('local_classifier_obj', 'LocalClassifier', 'classifier;');
        Generator.addCode([`classifier.getJsonValue(${json}, ${key})`, Generator.ORDER_UNARY_POSTFIX]);
    }

    //% block="获取最后错误信息" blockType="reporter"
    export function getLastError(parameter: any, block: any) {
        Generator.addInclude('local_classifier_include', '#include <LocalClassifier.h>');
        Generator.addObject('local_classifier_obj', 'LocalClassifier', 'classifier;');
        Generator.addCode([`classifier.getLastError()`, Generator.ORDER_UNARY_POSTFIX]);
    }
    //% block="初始化百度AI API Key[API_KEY] Secret Key[SECRET_KEY]" blockType="command"
    //% API_KEY.shadow="string" API_KEY.defl="your_api_key"
    //% SECRET_KEY.shadow="string" SECRET_KEY.defl="your_secret_key"
    export function initBaiduAI(parameter: any, block: any) {
        let apiKey = parameter.API_KEY.code;
        let secretKey = parameter.SECRET_KEY.code;
        Generator.addInclude('baidu_face_include', '#include <BaiduFaceAI.h>');
        Generator.addObject('baidu_face_obj', 'BaiduFaceAI', 'baiduFace;');
        Generator.addCode(`baiduFace.init(${apiKey}, ${secretKey});`);
    }

    //% block="百度AI 创建人脸库 库名[GROUP] 成功?" blockType="boolean"
    //% GROUP.shadow="string" GROUP.defl="group1"
    export function createFaceGroup(parameter: any, block: any) {
        let group = parameter.GROUP.code;
        Generator.addInclude('baidu_face_include', '#include <BaiduFaceAI.h>');
        Generator.addObject('baidu_face_obj', 'BaiduFaceAI', 'baiduFace;');
        Generator.addCode([`baiduFace.createFaceGroup(${group})`, Generator.ORDER_UNARY_POSTFIX]);
    }

    //% block="百度AI 注册人脸 图片Base64[IMAGE] 姓名[NAME] 到库[GROUP] 成功?" blockType="boolean"
    //% IMAGE.shadow="string" IMAGE.defl=""
    //% NAME.shadow="string" NAME.defl="user1"
    //% GROUP.shadow="string" GROUP.defl="group1"
    export function addFace(parameter: any, block: any) {
        let image = parameter.IMAGE.code;
        let name = parameter.NAME.code;
        let group = parameter.GROUP.code;
        Generator.addInclude('baidu_face_include', '#include <BaiduFaceAI.h>');
        Generator.addObject('baidu_face_obj', 'BaiduFaceAI', 'baiduFace;');
        Generator.addCode([`baiduFace.addFace(${image}, ${name}, ${group})`, Generator.ORDER_UNARY_POSTFIX]);
    }

    //% block="百度AI 搜索人脸 图片Base64[IMAGE] 在库[GROUP] 匹配结果" blockType="reporter"
    //% IMAGE.shadow="string" IMAGE.defl=""
    //% GROUP.shadow="string" GROUP.defl="group1"
    export function searchFace(parameter: any, block: any) {
        let image = parameter.IMAGE.code;
        let group = parameter.GROUP.code;
        Generator.addInclude('baidu_face_include', '#include <BaiduFaceAI.h>');
        Generator.addObject('baidu_face_obj', 'BaiduFaceAI', 'baiduFace;');
        Generator.addCode([`baiduFace.searchFace(${image}, ${group})`, Generator.ORDER_UNARY_POSTFIX]);
    }

    //% block="百度AI 表情识别 图片Base64[IMAGE]" blockType="reporter"
    //% IMAGE.shadow="string" IMAGE.defl=""
    export function analyzeEmotion(parameter: any, block: any) {
        let image = parameter.IMAGE.code;
        Generator.addInclude('baidu_face_include', '#include <BaiduFaceAI.h>');
        Generator.addObject('baidu_face_obj', 'BaiduFaceAI', 'baiduFace;');
        Generator.addCode([`baiduFace.analyzeEmotion(${image})`, Generator.ORDER_UNARY_POSTFIX]);
    }

 
}
