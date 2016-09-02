function match(image1, image2, imgname)%输入图像1和图像2的地址，以及图像文件名 
% str1='H:\intrinsic_image_work_record\data_NEW\';
str1 = 'H:\intrinsic_image_work_record\data_0506_I2\';
%mkdir(str1,imgname);

%fa , fb中存了关键点坐标，前两个是x,y
imga = imread(image1);
imgb = imread(image2);
[imgh, imgw, channels] = size(imga);

[fa, da] = vl_sift(im2single(rgb2gray(imga)));
[fb, db] = vl_sift(im2single(rgb2gray(imgb)));

[pathstr1, name1, ext1] = fileparts(image1);%分别得到路径，名称和后缀名
[pathstr2, name2, ext2] = fileparts(image2);

%得到匹配点
[matches, scores] = vl_ubcmatch(da, db) ;

[drop, perm] = sort(scores, 'descend');
matches = matches(:, perm);
scores = scores(perm);

[height,matchnum] = size(matches);
[feaheight,featurenum1] = size(fa);
[feaheight,featurenum2] = size(fb);
%%-----------
features = '\features_';
match_string = '\matches';
resultname = '\result_';
txt = '.txt';

%----生成匹配特征点坐标存储路径----
% fidpath1 = [str1,imgname,features,name1,txt];
% fidpath2 = [str1,imgname,features,name2,txt];
matchpath = [str1,imgname,match_string,txt];
% resultpath = [str1, imgname, resultname, ext1];

% fid1 = fopen(fidpath1,'wt');
% fid2 = fopen(fidpath2,'wt');
fid_match = fopen(matchpath,'wt');

%%------------

for i = 1: matchnum
    xa(1,i) = fix( fa(1, matches(1,i)));
    ya(1,i) = fix( fa(2, matches(1,i)));
   
    xb(1,i) = fix( fb(1, matches(2,i)));
    yb(1,i) = fix( fb(2, matches(2,i)));
end
%%----除去不对的点----
k = 1;
for i = 1:matchnum
    if(abs(ya(1,i)-yb(1,i))<50 && abs(xa(1,i)-xb(1,i))<600)
        img1x(1,k) = xa(1,i);
        img1y(1,k) = ya(1,i);
        img2x(1,k) = xb(1,i);
        img2y(1,k) = yb(1,i);
        k=k+1;
    end
end
%----------------------
% %-------------show match result---------------
% img = [imga, imgb];
% figure;
% imshow(img);
% hold on
% for i = 1:k-1
%     plot([img1x(1,i), img2x(1, i) + imgw], [img1y(1,i), img2y(1,i)]);
% end
% saveas(gcf, resultpath);
% hold off
% %----------------------
% fprintf(fid1,'%d\n',featurenum1);
% fprintf(fid2,'%d\n',featurenum2);
fprintf(fid_match,'%d\n',k-1);

% for i = 1:featurenum1
%     fprintf(fid1,'%d %d\n',fix(fa(1,i)), fix(fa(2,i)));
% end
% 
% for i = 1:featurenum2
%     fprintf(fid2,'%d %d\n',fix(fb(1,i)), fix(fb(2,i)));
% end

for i = 1:k-1
    fprintf(fid_match,'%d %d %d %d\n', img1x(1,i), img1y(1,i), img2x(1,i), img2y(1,i));
end

% fclose(fid1);
% fclose(fid2);
fclose(fid_match);
hold off;



