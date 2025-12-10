//% color="#AA278D" iconWidth=50 iconHeight=40
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

    //% block="OCR识别 图片Base64[IMAGE] 返回文本" blockType="reporter"
    //% IMAGE.shadow="string" IMAGE.defl=""
    export function ocrRecognize(parameter: any, block: any) {
        let image = parameter.IMAGE.code;
        Generator.addInclude('xf_ocr_include', '#include <XunfeiOCRAPI.h>');
        Generator.addObject('xf_ocr_obj', 'XunfeiOCRAPI','xunfeiOCR;');
        Generator.addCode([`xunfeiOCR.recognize(${image}).text`, Generator.ORDER_UNARY_POSTFIX]);
    }

    //% block="人脸识别 表情 图片Base64[IMAGE]" blockType="reporter"
    //% IMAGE.shadow="string" IMAGE.defl=""
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
}
