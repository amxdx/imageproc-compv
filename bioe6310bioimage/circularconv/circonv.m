function fg = circonv(f,g)
fg = zeros(length(f),1);
for i = 1:length(f)
    for j = 1 : length(g)
        if (i+1-floor(length(g)/2)<=0)
        fg(i) = fg(i)+ f(length(f)+1-j+floor(length(g/2)))*flip(g(j));
        elseif (i+1+floor(length(g)/2)> length(f))
            fg(i) = fg(i) + f(i+2-length(f)-j+floor(length(g/2)))*flip(g(j));
        else
            fg(i) = fg(i) + f(i+2-j+floor(length(g)/2))*flip(g(j));
        end
    end
end

