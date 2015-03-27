#!/usr/local/bin/python

"""
Sirak Gizaw
35273123
"""
 
import Image
import array
import os 
import random
import math
import numpy as np
#import matplotlib.pyplot as plt
from numpy import matrix
from numpy import linalg



########################################
def down_sample(image_files):
	w = int(0.30*195);
	h = int(0.30*231);
	for image in image_files:
		f = Image.open("Data/"+image);
		new = f.resize((w, h));
		new.save("NewImages2/"+image);
	
"""
select_samples - COMPLETED
input: pics - a list of numbers to select from the images 
output: a list of the sample file names
"""
def select_samples(pics):
   images = os.listdir('NewImages2');
   images.sort();
   sample_files = [];
   for c in range(15):
       for pic in pics:
           sample_files.append(images[(c*11)+(pic-1)]);
   sample_files.sort();
   return sample_files;
   
########################################   

"""
vect_image -
input: an image object (opened)
output: an array representing vectorized form of the image's pixels
"""
def vect_image(image):
	pic_Mat = image.load();
	w, h = image.size;
	pic_Arry = array.array('H');
#	print "From Vect_image: w="+str(w)+" h="+str(h);
	for x in range(w):
		for y in range(h):
			pic_Arry.append(pic_Mat[x, y]);

	return pic_Arry;

########################################
"""
Calculate the mean face from the sample matrix
"""
def mean_face1(sample_mat):
	h, w = sample_mat.shape;
#	total = np.zero((h));
	total = [0] * h;
	mean = total;
	temp = array.array('H');
	for y in range(w):
		for x in range(h):
			total[x] = total[x] + sample_mat[x, y];
#	print len(sample_mat);		 	
	for i in range(h):
		mean[i] = total[i]/float(w);
	return mean;

########################################
"""
Import sample files into a matrix and return
"""
def imp_arrays(samples, count):
#	sample_Mat = [0]*count;	#list of arrays
	image = Image.open("NewImages2/"+samples[0]);
	temp = vect_image(image);
#	print len(temp);
	sample_Mat = matrix(np.zeros((len(temp), count)));
#	sample_Mat = np.zeros((len(temp), count));
	i = 0;
	for s in range(len(samples)):
		image = Image.open("NewImages2/"+samples[s]);
		temp = vect_image(image);
		for x in range(len(temp)):
			sample_Mat[x,s] = temp[x];
		#i = i + 1;
	return sample_Mat;


########################################
"""
Substrate sample mean from sample data
return z matrix 
"""

def substrate_mean(mean, sample_mat):
	z = sample_mat;
#	z = np.zero((len(sample_mat[:,0]), len(sample_mat[0,:])))
	h, w = sample_mat.shape;
#	z = [0] * len(sample_mat);
#	temp = [0] * len(sample_mat[0]);
	count = 0;
	for y in range(w):
		for x in range(h):
			z[x,y] = sample_mat[x,y] - mean[x];
	
	return z; 


########################################
"""
Compute Scatter matrix
"""
def scatter_compute(z_mat):#, mean):
	h, w = z_mat.shape;
	scat_mat = np.zeros((h,h));
#	scat_mat = matrix(np.zeros((h,h)));
	for i in range(w):
#		scat_mat += np.outer(z_mat[:,i],z_mat[:,i]);
		scat_mat = scat_mat + (z_mat[:,i].dot((z_mat[:,i]).T));
	
	return scat_mat;

########################################
def eigen_compute(s_mat, D):
	e_vals, e_vects = np.linalg.eig(s_mat);
	evl_ret = [e_vals[0]]*D;
#	evc_ret = [e_vects[:,0]]*D;
	evc_ret = matrix(np.zeros((len(e_vects[:,0]), D)));
	eig = [(0,0)] * len(e_vals);
	for i in range(len(e_vals)):
		eig[i] = (np.abs(e_vals[i]), e_vects[:,i]);
		
	eig =  sorted(eig, key = lambda e:e[0], reverse=True);

#	eig.sort(); eig.reverse();
	for i in range(D):
		evl_ret[i] = eig[i][0];
#		print eig[i][1];
#		for h in range(len(eig[i][1])):
		evc_ret[:,i] = eig[i][1][:,0];

	return evl_ret, evc_ret;		
	
########################################	
def wt_compute(M, z_mat):
	M = M.T;
#	print M;
#	print M.shape;
	M_h, M_w = M.shape;
	z_h, z_w = z_mat.shape;
	w_mat = matrix(np.zeros((M_h, z_w)));
#	w_vec = np.zeros((M_h));
	for x in range(z_w):
		w_mat[:,x] = M.dot((z_mat[:,x]));
		
#	print w_mat.shape;
	return w_mat;		

########################################	


def nn_classifier(wt_tr_mat, wt_tst_mat):
	tst_h, tst_w = wt_tst_mat.shape;
	tr_w = wt_tr_mat.shape[1];
	assign_vect = [0]*tst_w;	 #assignment of the test weight vectors to the training weights
	for tst_x in range(tst_w):
		min_dist = 10000000000;
		i = 0;			
		for tr_x in range(tr_w):
			dist = 0; 
			for y in range(tst_h):
				dist = dist + (wt_tr_mat[y, tr_x] - wt_tst_mat[y, tst_x])**2 ;
			dist = math.sqrt(dist);
			if dist < min_dist:
				min_dist = dist;
				i = tr_x;		# assign it the test vectors index;
		assign_vect[tst_x] = i;
		
	return assign_vect;	#assign_vect[i] = tst_sample[i]'s assigned tr_sample with minimum distance 

########################################	

def calc_accuracy(assign_vect, tst_subjects, tr_subjects):
	"""
 	assign_vect is aligned with tst_subjects - a_v[i]  = tst[i]'s classification - index in tr_file
 	tr_subjects[a_v[i]] will be checked if its the same person as tst[i]. 
 	"""
 	count = 0;
 	for i in range(len(assign_vect)):
# 		print "Test " + tst_subjects[i] + " assigned to: " + tr_subjects[assign_vect[i]];
 		if tst_subjects[i] == tr_subjects[assign_vect[i]]:
 			count = count + 1;
 			
 	print "error rate: " + str((len(assign_vect)-count)/float(len(assign_vect)));
 	return (len(assign_vect)-count)/float(len(assign_vect));
 		

########################################	

def run(N, K):
	subjects = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11];

	##### training and testing sample selection
	train_s = random.sample(xrange(1,12), N);
	print "p = " + str(train_s);
#	train_s = [1]
	test_s = list(set(subjects) - set(train_s));
	
	##### training file import and weight calculation
	tr_files = select_samples(train_s);
#	print "List of training files: "
#	print tr_files;
	tr_subjects = [0]*len(tr_files);
	i = 0;
	for f_name in tr_files:
		tr_subjects[i] = (f_name.split("."))[0]; 
		i = i + 1;
		
	tr_samp_mat = imp_arrays(tr_files, len(train_s)*15);
	tr_mean = mean_face1(tr_samp_mat);
	z_mat = substrate_mean(tr_mean, tr_samp_mat);
	scat = scatter_compute(z_mat);
	
	####TESTing:
	tst_files = select_samples(test_s);

	tst_subjects = [0]*len(tst_files);
	i = 0;
	for f_name in tst_files:
		tst_subjects[i] = (f_name.split("."))[0]; 
		i = i + 1;

	tst_samp_mat = imp_arrays(tst_files, len(test_s)*15); #matrix of test samples
	p_mat = substrate_mean(tr_mean, tst_samp_mat); #normalization of all the test samples

	err_rate = [0.]*len(K);#, 0., 0., 0., 0., 0., 0., 0.];
#	K = [10, 20, 30];#, 40, 50, 60, 70, 80, 90, 100];	#for each experiment 10 different d values 
	for d in range(len(K)):
		print "d = " + str(K[d]);
		e_vals, e_vect_mat = eigen_compute(scat, K[d]); 
		wt_tr_mat = wt_compute(e_vect_mat, z_mat);
		wt_tst_mat = wt_compute(e_vect_mat, p_mat);	#new Weight
		asgn = nn_classifier(wt_tr_mat, wt_tst_mat);
		err_rate[d] =	calc_accuracy(asgn, tst_subjects, tr_subjects);
 
	return err_rate

########################################	


def fishr_f(N, K): 
	p = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11];
	##### training and testing sample selection
	train_s = random.sample(xrange(1,12), N);
	print "p = " + str(train_s);
	test_s = list(set(p) - set(train_s));
	##### training file import and weight calculation
	tr_files = select_samples(train_s);
	A_tr_subjects = [0]*len(tr_files);
	i = 0;
	for f_name in tr_files:
		A_tr_subjects[i] = (f_name.split("."))[0]; 
		i = i + 1;
	
	A_tr_sampmat = imp_arrays(tr_files, len(train_s)*15);
	A_tr_mean = mean_face1(A_tr_sampmat);
	###Classes#### A_tr_sampmat corresponds to the file order, which is sorted. The 1st N corresponde to sub1...
	Cl_means = [A_tr_mean]*15;  #subjects; 
	cls = np.array(np.zeros((len(A_tr_mean), N)));
	clss = [cls]*15;
#	cl_map = [(Cl_means, cls)]*15;
	for i in range(15):
		Cl_means[i] = mean_face1(A_tr_sampmat[:,(N*i):(N*i)+N]);
		clss[i] = A_tr_sampmat[:,(N*i):(N*i)+N]; 
		
	b_scat_mat = np.zeros((len(A_tr_mean),len(A_tr_mean)));
	
	#between class scatter

	for m in range(15):	#15 classes each with N samples 
		diff = A_tr_mean;
		for x in range(len(A_tr_mean)):
			diff[x] = Cl_means[m][x] - A_tr_mean[x]; 
		b_scat_mat = b_scat_mat + N*((np.array(diff)).dot((np.array(diff)).T));
		
#	print b_scat_mat.shape;
	
	#within-class scatter matrix
	Sw = np.zeros((len(A_tr_mean),len(A_tr_mean)));
	for c in range(15):
		sm = np.zeros((len(A_tr_mean),len(A_tr_mean)));
		for i in range(N):		# within the class
			diff = A_tr_mean;
			for x in range (len(A_tr_mean)):
				diff[x] = clss[c][x, i] - Cl_means[c][x];
			sm = sm + ((np.array(diff)).dot((np.array(diff)).T));	
		Sw = Sw + sm	
#	print Sw;
#	print Sw.shape;
	
	
	z = substrate_mean(A_tr_mean, A_tr_sampmat);
 	zz, Wpca = eigen_compute(scatter_compute(z), (N*15)-15);	#no. of samples - no of classes
#	print Wpca.shape;
	
#	mx = (Wpca.T) * (b_scat_mat) * Wpca;
#	mn = (Wpca.T) * (Sw) * Wpca;
	
	mx = (Wpca.T).dot(b_scat_mat).dot(Wpca);
	mn = (Wpca.T).dot(Sw).dot(Wpca);

#	u = np.linalg.inv(mn) * mx;
	u = mx.dot(np.linalg.inv(mn));
	
#	print str(mx.shape) + str(mn.shape);
#	print str(u.shape);
	

	
	###Testing
	tst_files = select_samples(test_s);

	tst_subjects = [0]*len(tst_files);
	i = 0;
	for f_name in tst_files:
		tst_subjects[i] = (f_name.split("."))[0]; 
		i = i + 1;

	tst_samp_mat = imp_arrays(tst_files, len(test_s)*15); #matrix of test samples
	p_mat = substrate_mean(A_tr_mean, tst_samp_mat); #normalization of all the test samples
	
	
	err_rate = [0.]*len(K);#, 0., 0., 0., 0., 0., 0., 0.];
#	K = [10, 20, 30];#, 40, 50, 60, 70, 80, 90, 100];	#for each experiment 10 different d values 
	for d in range(len(K)):
		print "d = " + str(K[d]);
		zz, Wfld = eigen_compute(u,K[d]);
		Wopt = Wfld.T.dot(Wpca.T);
		print str(Wopt.shape);
		Wopt = Wopt.T;
		print str(Wopt.shape);
		wt_tr_mat = wt_compute(Wopt, z);
		wt_tst_mat = wt_compute(Wopt, p_mat);
		asgn = nn_classifier(wt_tr_mat, wt_tst_mat);
		err_rate[d] =	calc_accuracy(asgn, tst_subjects, A_tr_subjects);
 
	return err_rate
		
#	return;
#################################
def main(N, K):
#	print "N = " + str(N) + "\n";
	errs = [[0.]*len(K)]*10;
	for exp in range(10):	#10 experiements for N; each with randomly selected N 
		print "N = " + str(N) + " Experiment: " + str(exp+1);
		errs[exp] = run(N, K);
#		errs[exp] = fishr_f(N, K);
	print errs;
	
	avg_err = [0.]*len(K);
	for d in range(len(K)):	# for each d
		s = 0.;
		for exp in range(len(errs)):	# for each exp add the errors corresponding to that d
			s = s + errs[exp][d];
		avg_err[d] = s/float(len(errs));	
	
	print "\nAverage errors: ";
	print avg_err;
	print "d = "
	print K;
	"""
	plt.plot(K, avg_err, 'ro')
	plt.axis([0, 80, 0, max(avg_err)+0.1]);
	plt.grid(True);
	plt.title('Average Error Rate VS Dimensions for N = ' + str(N));
	plt.ylabel('Average Error Rate');
	plt.xlabel('Dimensions(d\')');
	plt.show();
	"""
#print run(3);
"""
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#ONLY RUN ONCE to resize
ps = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11];
a_files = select_samples(ps);
down_sample(a_files);
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%
"""
K = [10, 20, 30, 40, 50, 60, 70];	#for each experiment 10 different d values 
main(3, K);
print "\nFINISHED N = 3\n\n"
#main(5, K)
#print "\nFINISHED N = 5\n\n"
#main(7, K)
#print "\nFINISHED N = 7\n\n"

