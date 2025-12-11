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


    
}
