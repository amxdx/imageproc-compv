function fcg = circonv2(f,g)
[r,c] = size(f);
[R,C] = size(g);
fcg = zeros(r,c);
r1 = floor(R/2);
c1 = floor(C/2);
for i = 1:r
    for j = 1:c 
       
        for k = -r1:r1
            for l = -c1:c1 
                
                rt1 = i+k;
                ct1 = j+l;
                
                if (rt1<1 && ct1< 1)
                    fcg(i,j) = fcg(i,j) + f(r-rt1,c-ct1)*g(r1+1-k,c1+1-l);
                   
                elseif(rt1<1 && ct1<c+1)
                    fcg(i,j) = fcg(i,j) + f(r-rt1,ct1)*g(r1+1-k,c1+1-l);
                   
                elseif(ct1<1 && rt1<r+1) 
                    fcg(i,j) = fcg(i,j) + f(rt1,c-ct1)*g(r1+1-k,c1+1-l);
                   
                elseif (rt1>r && ct1>c)
                    fcg(i,j) = fcg(i,j) + f(rt1-r,ct1-c)*g(r1+1-k,c1+1-l);
                   
                elseif (rt1>r && ct1>=1)
                    fcg(i,j) = fcg(i,j) + f(rt1-r,ct1)*g(r1+1-k,c1+1-l);
                   
                elseif(ct1>c && rt1>=1)
                    fcg(i,j) = fcg(i,j) + f(rt1,ct1-c)*g(r1+1-k,c1+1-l);
                   
                elseif (rt1<1 && ct1>c) 
                    fcg(i,j) = fcg(i,j) + f(r-rt1,ct1-c)*g(r1+1-k,c1+1-l);
                elseif (ct1<1 && rt1>r) 
                    fcg(i,j) = fcg(i,j) + f(rt1-r,c-ct1)*g(r1+1-k,c1+1-l);    
                else
                    fcg(i,j) = fcg(i,j) + f(i+k,j+l)*g(r1+1-k,c1+1-l);
                end
                    
            end
        end
    end
end

