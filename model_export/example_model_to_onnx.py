import torch
import torchvision.models as models

# 下载resnet50预训练模型
resnet50 = models.resnet50(pretrained=True)
# 下载mobilenet_v2预训练模型
mobilenet_v2 = models.mobilenet_v2(pretrained=True)

# q: 保存pth和onnx前是否要设置模型为eval模式? a: 是的, 因为有些层在训练和推理时行为不同, 比如BatchNorm和Dropout
resnet50.eval()
mobilenet_v2.eval()

# 打印分类类别数
print("ResNet50 classes:", resnet50.fc.out_features)
print("MobileNetV2 classes:", mobilenet_v2.classifier[1].out_features)

# 打印模型输入尺寸和输出尺寸
print("ResNet50 input size:", resnet50.fc.in_features)
print("ResNet50 output size:", resnet50.fc.out_features)
print("MobileNetV2 input size:", mobilenet_v2.classifier[1].in_features) # q: 为什么是classifier[1]? a: 因为mobilenet_v2的classifier是一个Sequential, 里面有两个层, 第一个是Dropout, 第二个是Linear
print("MobileNetV2 output size:", mobilenet_v2.classifier[1].out_features)

# 打印类别名, 其实就是ImageNet的1000个类别
# print("ResNet50 class names:", models.ResNet50_Weights.IMAGENET1K_V1.meta['categories'])
# print("MobileNetV2 class names:", models.MobileNet_V2_Weights.IMAGENET1K_V1.meta['categories'])

# 将类别名保存到本地文件
with open('imagenet_classes.txt', 'w') as f:
    for class_name in models.MobileNet_V2_Weights.IMAGENET1K_V1.meta['categories']:
        f.write(class_name + '\n')

# 保存模型到本地文件
torch.save(resnet50.state_dict(), 'resnet50_pretrained.pth')
torch.save(mobilenet_v2.state_dict(), 'mobilenet_v2_pretrained.pth')

# 将模型转换为ONNX格式并保存
dummy_input = torch.randn(1, 3, 224, 224)  # 模拟输入张量
torch.onnx.export(resnet50, dummy_input, 'resnet50_pretrained.onnx', opset_version=11) 
torch.onnx.export(mobilenet_v2, dummy_input, 'mobilenet_v2_pretrained.onnx', opset_version=11) 

# 将onnx转换为engine的命令
# trtexec --onnx=resnet50_pretrained.onnx --saveEngine=resnet50_pretrained.engine --fp16
# trtexec --onnx=mobilenet_v2_pretrained.onnx --saveEngine=mobilenet_v2_pretrained.engine --fp16
