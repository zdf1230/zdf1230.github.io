// SGU 101 -- Domino
import java.util.Scanner;


public class Solution {

	private static final int Maxm = 100 + 10;
	private static final int Maxn = 7;
	private static int n;
	private static int l = 1;
	private static int sum = 0;
	private int pre[], last[], other[], w[];
	private int id[], zt[];
	private boolean v[];
	
	Solution() {
		pre = new int [Maxm << 1];
		last = new int [Maxn];
		other = new int [Maxm << 1];
		w = new int [Maxm << 1];
		id = new int [Maxn];
		zt = new int [Maxm << 1];
		v = new boolean [Maxm << 1];
	}
	
	private void Add_edge(int p, int q, int num) {
		pre[++l] = last[p]; last[p] = l; other[l] = q; w[l] = num;
		pre[++l] = last[q]; last[q] = l; other[l] = p; w[l] = -num;
	}
	
	private void Init() {
		Scanner conin = new Scanner(System.in);
		n = conin.nextInt();
		for (int i = 1; i <= n; ++i) {
			int x = conin.nextInt();
			int y = conin.nextInt();
			Add_edge(x, y, i);
			++id[x]; ++id[y];
		}
		conin.close();
	}
	
	private void Dfs(int p) {
		for (int q = last[p]; q > 0; q = pre[q]) {
			if (!v[q]) {
				v[q] = true;
				v[q ^ 1] = true;
				Dfs(other[q]);
				zt[++sum] = w[q];
			}
		}
	}
	
	private void Work() {
		int cnt = 0, first = -1;
		for (int i = 0; i < Maxn; ++i)
			if ((id[i] & 1) == 1) {
				++cnt;
				first = i;
			}
		if (cnt != 0 && cnt != 2) {
			System.out.println("No solution");
			return;
		}
		if (first != -1)
			Dfs(first);
		else
			for (int i = 0; i < Maxn; ++ i)
				if (id[i] != 0) {
					Dfs(i);
					break;
				}
		if (sum != n) {
			System.out.println("No solution");
			return;
		}
		for (int i = sum; i >= 1; --i) {
			if (zt[i] > 0)
				System.out.printf("%d +\n", zt[i]);
			else
				System.out.printf("%d -\n", -zt[i]);
		}
	}
	
	public static void main(String[] args) {
		Solution work = new Solution();
		work.Init();
		work.Work();
	}

}
