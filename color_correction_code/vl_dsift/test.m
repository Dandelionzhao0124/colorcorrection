run('G:/vlfeat/vlfeat-0.9.20-bin/vlfeat-0.9.20/toolbox/vl_setup');

%path = 'H:\intrinsic_image_work_record\data_NEW\';
path = 'H:\intrinsic_image_work_record\data_0506_I2\';
fid = fopen([path,'Art.txt'],'rt');

% for i = 1:50
for i = 1:1
    str = fgetl(fid);%获取图像文件名
    S = regexp(str, ' ', 'split');
    imagename = S{1};
    imgpath1 = [path, imagename, '\', S{2}, '.png'];
    imgpath2 = [path, imagename, '\', S{3}, '_corr_Lab.png'];
    match(imgpath1, imgpath2, imagename);
end
