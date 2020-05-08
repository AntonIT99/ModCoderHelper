#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

using namespace std;

string* read_directory(const char* path, int* nb_files);

int main()
{
    cout << "Listing Files:" << endl;

    DIR* dirp = opendir("model\\");
    DIR* dirp0 = opendir("model\\");
    struct dirent * dp;

    int nbfiles = 0;
    int n = 0;
    char *extension;

    //calcul nombre de fichiers java
    while ((dp = readdir(dirp0)) != NULL)
    {
        extension = strrchr(dp->d_name, '.');
        if(extension && strcmp(extension, ".java") == 0){
            nbfiles++;
        }
    }
    string filenames[nbfiles];

    while ((dp = readdir(dirp)) != NULL)
    {
        string s(dp->d_name);
        extension = strrchr(dp->d_name, '.');
        if(extension && strcmp(extension, ".java") == 0){
            filenames[n] = s.substr(0, s.size()-5);
            n++;
        }
    }

    string modelnames[nbfiles];

    for (int i = 0; i < nbfiles; i++)
    {
        modelnames[i] = filenames[i].substr(5, filenames[i].size()-1);
        cout << "Model : " << modelnames[i] << endl;
    }

    FILE* render = NULL;
    FILE* entity = NULL;
    FILE* entityregistry = fopen("ModEntityRegistry.txt", "w+");
    FILE* clientproxy = fopen("Clientproxy.txt", "w+");

    string path_render[nbfiles];
    string path_entity[nbfiles];
    string ressource_path;


    for (int i = 0; i < nbfiles; i++)
    {
        path_render[i] = "render\\Render";
        path_render[i].append(modelnames[i]);
        path_render[i].append(".java");
        path_entity[i] = "entity\\Entity";
        path_entity[i].append(modelnames[i]);
        path_entity[i].append(".java");

        //ressource_path = "ressources\\";
        //ressource_path.append(modelnames[i]);
        //_mkdir(ressource_path.c_str());

        entity = fopen(path_entity[i].c_str(), "w+");
        fprintf(entity, "package com.wolffsmod.entity;\n\n");
        fprintf(entity, "import net.minecraft.entity.EntityCreature;\n");
        fprintf(entity, "import net.minecraft.world.World;\n\n");
        fprintf(entity, "public class Entity%s extends EntityCreature {\n", modelnames[i].c_str());
        fprintf(entity, "	public Entity%s(World w) {\n", modelnames[i].c_str());
        fprintf(entity, "		super(w);\n");
        fprintf(entity, "		this.setSize(3.5F, 2.5F);\n");
        fprintf(entity, "	}\n");
        fprintf(entity, "}\n");

        fclose(entity);

        render = fopen(path_render[i].c_str(), "w+");
        fprintf(render, "package com.wolffsmod.render;\n\n");
        fprintf(render, "import net.minecraft.client.model.ModelBase;\n");
        fprintf(render, "import net.minecraft.client.renderer.entity.RenderLiving;\n");
        fprintf(render, "import net.minecraft.entity.Entity;\n");
        fprintf(render, "import net.minecraft.entity.EntityLivingBase;\n");
        fprintf(render, "import net.minecraft.util.ResourceLocation;\n");
        fprintf(render, "import org.lwjgl.opengl.GL11;\n");
        fprintf(render, "import com.wolffsmod.Strings;\n");
        fprintf(render, "import com.wolffsmod.entity.Entity%s;\n", modelnames[i].c_str());
        fprintf(render, "import cpw.mods.fml.relauncher.Side;\n");
        fprintf(render, "import cpw.mods.fml.relauncher.SideOnly;\n\n");
        fprintf(render, "@SideOnly(Side.CLIENT)\n");
        fprintf(render, "public class Render%s extends RenderLiving\n", modelnames[i].c_str());
        fprintf(render, "{\n");
        fprintf(render, "    private static final ResourceLocation Texture = new ResourceLocation(Strings.MODID + \":textures/entity/%s/%s.png\");\n\n", modelnames[i].c_str(), modelnames[i].c_str());
        fprintf(render, "    public Render%s(ModelBase par1ModelBase, float par2)\n", modelnames[i].c_str());
        fprintf(render, "    {\n");
        fprintf(render, "        super(par1ModelBase, par2);\n");
        fprintf(render, "    }\n\n");
        fprintf(render, "    protected void preRenderCallback(EntityLivingBase entity, float f){\n");
        fprintf(render, "    	GL11.glTranslatef(0F, -2.1F, 0F);\n");
        fprintf(render, "    	GL11.glRotatef(180, 1, 0, 0);\n");
        fprintf(render, "    	GL11.glRotatef(270, 0, 1, 0);\n");
        fprintf(render, "    }\n\n");
        fprintf(render, "    protected ResourceLocation getEntityTexture(Entity%s par1Entity)\n", modelnames[i].c_str());
        fprintf(render, "    {\n");
        fprintf(render, "        return Texture;\n");
        fprintf(render, "    }\n\n");
        fprintf(render, "    protected ResourceLocation getEntityTexture(Entity par1Entity)\n");
        fprintf(render, "    {\n");
        fprintf(render, "        return this.getEntityTexture((Entity%s)par1Entity);\n", modelnames[i].c_str());
        fprintf(render, "    }\n");
        fprintf(render, "}\n");

        fclose(render);

    }

    for (int i = 0; i < nbfiles; i++)
    {
        fprintf(entityregistry, "import com.wolffsmod.entity.Entity%s;\n", modelnames[i].c_str());
    }
    for (int i = 0; i < nbfiles; i++)
    {
        fprintf(entityregistry, "		createEntity(Entity%s.class, \"[FlansMod] %s\", 0x356633, 0x0A8000);\n", modelnames[i].c_str(), modelnames[i].c_str());
    }

    for (int i = 0; i < nbfiles; i++)
    {
        fprintf(clientproxy, "import com.wolffsmod.entity.Entity%s;\n", modelnames[i].c_str());
        fprintf(clientproxy, "import com.wolffsmod.model.Model%s;\n", modelnames[i].c_str());
        fprintf(clientproxy, "import com.wolffsmod.render.Render%s;\n", modelnames[i].c_str());
    }

    for (int i = 0; i < nbfiles; i++)
    {
        fprintf(clientproxy, "		RenderingRegistry.registerEntityRenderingHandler(Entity%s.class, new Render%s(new Model%s(), 0));\n", modelnames[i].c_str(), modelnames[i].c_str(), modelnames[i].c_str());
    }

    fclose(entityregistry);
    fclose(clientproxy);

    closedir(dirp);
    closedir(dirp0);

    return 0;
}
