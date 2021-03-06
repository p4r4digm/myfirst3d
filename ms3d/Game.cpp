#include "Game.hpp"

#include "graphics/Model.hpp"

#include "utility/StringView.hpp"
#include "utility/Matrix.hpp"
#include "utility/Color.hpp"
#include "utility/String.hpp"

#include <assert.h>
#include "utility/Closure.hpp"

#include "utility/LispExpressions.hpp"
#include "utility/LispContext.hpp"
#include "utility/Assert.hpp"


namespace app {
   class Game::Impl {
      gfx::Renderer &m_renderer;
      plat::Window &m_window;

      gfx::Model *buildTestModel() {

         std::vector<gfx::FVF_Pos2_Tex2_Col4> vertices = {
            { { 0.0f, 0.0f },{ 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
            { { 1.0f, 0.0f },{ 1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } },
            { { 1.0f, 1.0f },{ 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f, 1.0f } },
            { { 0.0f, 1.0f },{ 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } }
         };

         std::vector<int> indices = { 0, 1, 3, 1, 2, 3 };

         return m_renderer.getModelFactory().create(
            vertices.data(), 
            vertices.size(), 
            indices.data(), 
            indices.size());
      }

      void _testxpression3() {
         auto res = lisp::parse(LISP_EXPR(
            for-each i in range-from(1 to 10)
               (print "value is \"" i "\" and ")
               (print "next value is " (+ i 1) "\n")
            )
         );

         lisp::Context c;

         c.evaluate(res);

         c.evaluate(lisp::parse(LISP_EXPR(print (+ 1 2) "\n")));
         c.evaluate(lisp::parse(LISP_EXPR(print (+ 1 2 3 4 5) "\n")));
         c.evaluate(lisp::parse(LISP_EXPR(print (+ 1 (+ 1 2)) "\n")));
         c.evaluate(lisp::parse(LISP_EXPR(print (+ (+ 1 2) (+ 1 2)) "\n")));

         c.evaluate(lisp::parse(LISP_EXPR(print :(print 1 :(print 2)) "\n")));
         c.evaluate(lisp::parse(LISP_EXPR(print (print 1 :(print 2)) "\n")));
         c.evaluate(lisp::parse(LISP_EXPR(print (:print 1 (print 2)) "\n")));
         c.evaluate(lisp::parse(LISP_EXPR(print (:print 1 :(print 2)) "\n")));

         c.evaluate(lisp::parse(LISP_EXPR(print "this " 0.0 " is \n a " :(print 1) " " SYMBOL " test " (1 2 3 4))));

         auto list = *res.list();
         auto foreach = *list[0].sym();
         auto i = *list[1].sym();
         auto in = *list[2].sym();

         auto rangelist = *list[3].list();
         auto rname = *rangelist[0].sym();
         auto one = *rangelist[1].i32();
         auto to = *rangelist[2].sym();
         auto ten = *rangelist[3].i32();

         auto printlist1 = *list[4].list();
         auto p1name = *printlist1[0].sym();
         auto p1openquote = *printlist1[1].str();
         auto p1i = *printlist1[2].sym();
         auto p1closequote = *printlist1[3].str();

         auto printlist2 = *list[5].list();
         auto p2name = *printlist2[0].sym();
         auto p2openquote = *printlist2[1].str();

         auto p2list = *printlist2[2].list();
         auto plus = *p2list[0].sym();
         auto p2i = *p2list[1].sym();
         auto p2one = *p2list[2].i32();


         int ashdjashdj = 5;
      }

      void _testxpressions2() {
         lisp::List list(4);
         list[0] = 5;
         list[1] = lisp::Str("wtf");
         list[2] = lisp::internSym("does this work?");
         list[3] = "wtfwtf";

         lisp::Expr listCopy(list);
         for (auto&& val : *listCopy.list())
         {
            if (auto i = val.i32())
            {
               printf("int: %d\n", *i);
            }
            else if (auto s = val.sym())
            {
               printf("sym: %s\n", (const char *)*s);
            }
            else if (auto st = val.str())
            {
               printf("str: %s\n", st->c_str());
            }
         }
      }


      void _testxpressions() {
         lisp::List list1;
         list1.push_back(lisp::Expr(45));

         class foo {
            int i[100];
         public:
            foo() { memset((int*)i, 0, 100 * sizeof(int)); }
            ~foo() {
               i[0] = 5;
            }
         };

         foo bar;

         lisp::Expr sexnil;
         lisp::Expr sexi(1);
         lisp::Expr sexf(2.0f);
         lisp::Expr sexstr("lol");
         lisp::Expr sexsymb(lisp::internSym("lolol"));
         lisp::Expr sexlist(std::move(list1));
         lisp::Expr sexfoo(bar);


         auto i = *sexi.i32();
         auto f = *sexf.f32();
         auto str = *sexstr.str();
         auto symb = *sexsymb.sym();
         auto list = *sexlist.list();
         auto bar2 = *sexfoo.obj<foo>();


         int j = i;
      }

   public:
      Impl(gfx::Renderer &r, plat::Window &w):m_renderer(r), m_window(w) {}
      ~Impl() {}

      gfx::Shader *m_shader;
      gfx::Model *m_model;
      gfx::Texture *m_texture;

      void start() {
         auto test1 = utl::internString("test");
         auto test2 = lisp::internSym("test");

         auto &r = m_renderer;

         m_model = buildTestModel();
         m_shader = r.getShaderFactory().create("assets/shaders.glsl", gfx::DiffuseTexture);

         gfx::TextureRequest request(utl::internString("assets/00.png"));

         m_texture = r.getTextureManager().get(request);

         _testxpressions();
         _testxpressions2();
         _testxpression3();

         lisp::Context context;

         auto evalName = lisp::internSym("thisIsATest");
         auto eval = lisp::createEvaluator([=](lisp::Expr &sxp, lisp::Context &context) {return lisp::Expr();});

         context.store(evalName, eval);

         if (auto evalExpr = context.load(evalName)) {
            if (auto e = evalExpr.obj<lisp::Evaluator>()) {
               auto &exec = **e;
               auto out = exec(lisp::Expr(), context);
            }
            
         }

         lisp::List heterogeneousListsLmao = { lisp::List{1, 2, 3, 4.5f}, 4, 6.5f, "fuuuuck you", lisp::internSym("ID"), std::make_shared<std::vector<std::string>>() };

         utl::Vector<int> foo;
         foo.push_back(1);
         foo.push_back(2);

         for (auto &&f : foo) {
            int i = f;
            int j = 5;
         }

         lisp::List list1;
         list1.push_back(lisp::Expr(45));
         {
            lisp::List list2(std::move(list1));
            list2.push_back(lisp::Expr(186));

            for (auto && item : list2) {
               if (auto i = item.i32()) {
                  int blah = *i;
                  int blahbla = 5;
               }

            }

            list1 = list2;
            auto list3 = std::move(list1);
            list1 = std::move(list3);
         }
         for (auto && item : list1) {
            if (auto i = item.i32()) {
               int blah = *i;
               int blahbla = 5;
            }

         }
      }
      
      void update() {

      }

      void render() {
         auto &r = m_renderer;

         auto uView = utl::internString("uViewMatrix");
         auto uModel = utl::internString("uModelMatrix");
         auto uColor = utl::internString("uColorTransform");
         auto uTexture = utl::internString("uTexMatrix");
         auto uTextureSlot = utl::internString("uTexture");
         

         r.viewport({ 0, 0, (int)r.getWidth(), (int)r.getHeight() });
         r.clear({ 1.0f, 0.0f, 0.0f, 1.0f });

         utl::Matrix modelTransform =
            utl::Matrix::translate({ 100.0f, 100.0f }) *
            utl::Matrix::scale({ 100.0f, 100.0f });

         utl::Matrix viewTransform =
            utl::Matrix::ortho(0.0f, (float)r.getWidth(), (float)r.getHeight(), 0.0f, 1.0f, -1.0f);

         utl::Matrix texTransform = utl::Matrix::identity();

         utl::ColorRGBAf colorTransform = { 1.0f, 1.0f, 1.0f, 1.0f };

         r.setShader(m_shader);
         r.setMatrix(uView, viewTransform);
         r.setMatrix(uModel, modelTransform);
         r.setMatrix(uTexture, texTransform);
         r.setColor(uColor, colorTransform);

         r.bindTexture(m_texture, 0);
         r.setTextureSlot(uTextureSlot, 0);

         r.renderModel(m_model);

         r.finish();
      }

      void processInput() {
         m_window.pollEvents();
      }

      void step() {
         update();
         render();
         processInput();
      }

      bool shouldStop() {
         return m_window.shouldClose();
      }
   };

   Game::Game(gfx::Renderer &r, plat::Window &w):pImpl(new Impl(r, w)) { }
   Game::~Game() { }

   void Game::start() { pImpl->start(); }
   void Game::step() { pImpl->step(); }
   bool Game::shouldStop() { return pImpl->shouldStop(); }
}