#include <stdio.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include "enumerate.h"


#define min(x, y)  (((x) < (y)) ? x : y)
#define RLIM	200	/*  maximum sample size */
#define KLIMIT	40	/*  maximum number of alleles  */

int r_obs[KLIMIT];  	/*  observed configuration */
int r[KLIMIT];		/*  sample configuration  */
int k;			/*  number of allelic classes */
int r_tot;		/* 	number of copies sampled= n  */
int r_top;		/*  highest value of r[i] */
int alpha[RLIM];    	/*  here so it will be set to zero  */
double tot_sum;		/*  sum of all coefficients */
double sig_sum;		/*  sum of significant coefficients  */
double F_sig_sum;	/*  sum for significant F values  */
double F_obs;		/*  homozygosity observed
                     */
double obs_value;	/*  coefficient for r_obs  */
long   double factors[RLIM];	/*  contains factorials  */
int Fsig, Esig;

slatkin_enum_result slatkin_enum( int r_obs[] ){
  int i;
  void config(int rt, int rmax, int ic);
  double ewens_form(int *r, int r_tot, double *mpt);
  double F(int *r); 
  double multiplicity;
  double theta_est(int k_obs, int n);
  void print_config(int *r);
  void fill_factors();
  slatkin_enum_result result;
  r_tot = 0;
  k = 1;
  while( r_obs[k] > 0 ){
		r_tot += r_obs[k];
    /* printf("k: %d  r_obs[k]: %d\n",k,r_obs[k]); */
    k++;
  }
  k = k-1;
	F_obs = F(r_obs);
  /* printf("\nn = %d, k = %d, theta = %g, F = %g\n", r_tot, k, theta_est(k, r_tot), F_obs); */
  if (r_tot >= RLIM)  {
    printf("n = %d is too large..\n", r_tot);
    exit(0);
  }
  if (k >= KLIMIT)  {
    printf("k = %d is too large.\n", k);
    exit(0);
  }
  for (i=1; i<k; i++)
    if (r_obs[i] < r_obs[i+1])  {
      print_config(r_obs);
      printf(" A valid configuration must be in decreasing order of allele counts.\n");
      exit(0);
  }
	r_top = r_tot - 1;
	Fsig = Esig = 0;
	fill_factors();
  obs_value = ewens_form(r_obs, r_obs[1], &multiplicity);
  config(r_tot, r_tot-k+1, 1);
  /* print_config(r_obs); */
  result.prob_slatkin_exact = sig_sum / tot_sum;
  result.prob_watterson = F_sig_sum / tot_sum;
  result.theta_estimate = theta_est(k, r_tot);
  return result;
}

int main(int argc, char *argv[]) {
	int i;
	void config(int rt, int rmax, int ic);
	double ewens_form(int *r, int r_tot, double *mpt);
	/* double F(int *r), multiplicity; */
	double theta_est(int k_obs, int n);
	void print_config(int *r);
	/* long start_time, finish_time, net_time; */
	void fill_factors();
	k = argc - 1;
	for (i=1; i<=k; i++)  {
		r_obs[i] = atoi(argv[i]);
		r_tot += r_obs[i];
    }
  slatkin_enum_result result = slatkin_enum( r_obs );
  printf("sl_exact : %6.3f\n",result.prob_slatkin_exact);
  printf("watterson: %6.3f\n",result.prob_watterson);
  printf("theta est: %6.3f\n",result.theta_estimate);
  return 0;
}
 
void config(int rt, int rmax, int ic)  {
    int r1, i;
    double ewens_form(int *r, int r_top, double *mpt), test_value;
    double F(int *r), F_test, multiplicity;
    void print_config(int *r);
    
    if (ic == k - 1)
        for (i=min(rmax,rt-1); i>=((rt%2)?(rt+1)/2:rt/2); i--) {
            r[ic] = i;
            r[ic+1] = rt - i;
            test_value = ewens_form(r, r_top, &multiplicity);
            tot_sum += multiplicity * test_value;
            F_test = F(r);
            if (test_value <= obs_value)
                sig_sum += multiplicity * test_value;
            if (F_test <= F_obs)
                F_sig_sum += multiplicity * test_value;
        }
    else  {
        for(r1=((rt%k)?rt/k+1:rt/k);r1<=(min(rmax,rt-k+ic+1));r1++)  { 
            if (ic == 1)
                r_top = r1;
            r[ic] = r1;
            config(rt-r1, r1, ic+1);
        }
    }
}  /*  end, config  */

void fill_factors()  {
	int i;
    
	factors[0] = 1.0;
	for(i=1; i<=r_tot; i++)
		factors[i] = i * factors[i-1];
}

void print_config(int *r) {
	int i;
    
	printf("(");
	for (i=1; i<k; i++)
		printf("%d,", r[i]);
	printf("%d)\n", r[k]);
}  /*  end, print_config  */

double ewens_form(int *r, int r_top, double *mpt)  {
	int i;
	void print_alpha(int a[RLIM], int r_top);
	double coef;
    
	for (i=1; i<=r_top; i++)
		alpha[i] = 0;
	for(i=1; i<=k; i++)
		alpha[r[i]]++;
	coef = 1.0;
	*mpt = factors[r_tot];
	for (i=1; i<=r_top; i++)
		if (alpha[i])  {
    		coef *= 1.0 / pow(i, alpha[i]);
    		*mpt /= factors[alpha[i]];
        }
	return coef;
}  /*  end, ewens_form  */

double F(int *r)  {
    int i;
    double sum;
    
    sum = 0.0;
    for (i=1; i<=k; i++)  sum += r[i] * r[i];
    return sum / (r_tot * r_tot);
}

double theta_est(int k_obs, int n)  {
    /*  Estimates theta = 4N*mu using formula 9.26 in Ewens' book  */
	double kval(double theta, int n);
	double xlow, xhigh, xmid;
	double eps;
	
	eps = 0.00001;
	xlow = 0.1;
	while (kval(xlow, n) > k_obs)
		xlow /= 10.0;
	xhigh = 10.0;
	while (kval(xhigh, n) < k_obs)
		xhigh *= 10.0;
	while ((xhigh - xlow) > eps)  {
		xmid = (xhigh + xlow) / 2.0;
		if (kval(xmid, n) > k_obs)
			xhigh = xmid;
		else
			xlow = xmid;
    }
	return xmid;
}  /*  end, theta_est  */

double kval(double x, int n)  {
	int i;
	double sum;
	
	sum = 0.0;
	for (i=0; i<n; i++)
		sum += x / (i + x);
	return sum;
}
